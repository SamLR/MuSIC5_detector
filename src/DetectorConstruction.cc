//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"
#include "Field.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"

#include "G4LogicalVolume.hh"
#include "G4AssemblyVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4SDManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"
#include "G4RunManager.hh"
#include "G4UserLimits.hh"

#include <vector>
#include <string.h>

DetectorConstruction::DetectorConstruction()
:f_myField(0), f_updated(false),
f_fname_sol("../../fieldmap/MuSIC5_detector/fieldmap_solenoid.txt"),
f_fname_dip("../../fieldmap/MuSIC5_detector/fieldmap_dipole.txt"),
f_dip_polarity(1.0),
f_degraderZ(1.0*mm/2.0), f_targetZ(0.5*mm/2.0),
f_u_limit (0), f_d_limit(0), f_deg_limit(0), f_st_limit(0)
{
    f_messenger = new DetectorConstructionMessenger(this);
    DefineMaterials();
    f_scint1Mat   = G4Material::GetMaterial("Scintillator");
    f_scint2Mat   = G4Material::GetMaterial("Scintillator");
    f_degraderMat = G4Material::GetMaterial("Polystyrene");
    f_targetMat   = G4Material::GetMaterial("Copper");
}

DetectorConstruction::~DetectorConstruction()
{
    delete f_myField;
    delete f_messenger;
    
    // pure elements
    delete Pb;
    delete Al;
    delete Mg;
    delete Cu;
    delete Air;
    delete Scint;
    delete Mylar;
    delete Polystyrene;
    delete Polyethylene;
    delete f_u_limit;
    delete f_d_limit;
    delete f_deg_limit;
    delete f_st_limit;
}

void DetectorConstruction::DefineMaterials()
{
    G4String symbol;
    G4double a, z, density, fractionmass;
    G4int ncomponents, natoms;
    
    // Elements for use in compounds/mixtures
    G4Element* H  = new G4Element("Hydrogen",symbol="H" , z= 1., a= 1.01*g/mole);
    G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z= 6., a= 12.01*g/mole);
    G4Element* N  = new G4Element("Nitrogen",symbol="N" , z= 7., a= 14.01*g/mole);
    G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
    // elements for use on their own
    Al = new G4Material("Aluminium", z=13.0, a= 26.98 *g/mole, density= 2.700*g/cm3);
    Pb = new G4Material("Lead",      z=82.0, a= 207.19*g/mole, density= 11.35*g/cm3);
    Mg = new G4Material("Magnesium", z=12.0, a= 24.305*g/mole, density= 1.738*g/cm3);
    Cu = new G4Material("Copper",    z=29.0, a= 63.546*g/mole, density= 8.94 *g/cm3);
    
    // mixtures and compounds
    // possibly Polyvinyltoluene. See pg322 of Mokhov (2001) for calculated stopping power
    Scint = new G4Material("Scintillator", density= 1.032*g/cm3, ncomponents=2);
    Scint->AddElement(C, natoms=9);
    Scint->AddElement(H, natoms=10);
    
    Mylar = new G4Material("Mylar", density= 1.397*g/cm3, ncomponents=3);
    Mylar->AddElement(C, natoms=10);
    Mylar->AddElement(H, natoms= 8);
    Mylar->AddElement(O, natoms= 4);
    
    Air = new G4Material("Air"  , density= 1.290*mg/cm3, ncomponents=2);
    Air->AddElement(N, fractionmass=0.7);
    Air->AddElement(O, fractionmass=0.3);
    
    // Want to make sure we have functional air/scint boundaries (mainly for MPPC)
    const int n_air_photons = 2;
    double air_photons[n_air_photons] = {2.0*eV, 3.6*eV};
    double air_rindex[n_air_photons]  = {1.0, 1.0};
    
    G4MaterialPropertiesTable* air_MPT = new G4MaterialPropertiesTable();
    air_MPT->AddProperty("RINDEX", air_photons, air_rindex, n_air_photons);
    Air->SetMaterialPropertiesTable(air_MPT);
    
    
    Polyethylene = new G4Material("Polyethylene",density= 0.94*g/cm3, ncomponents=2);
    Polyethylene -> AddElement(H,0.14);
    
    Polyethylene -> AddElement(C,0.86);
    Polystyrene = new G4Material("Polystyrene", density= 1.03*g/cm3, 2);
    Polystyrene->AddElement(C, 8);
    Polystyrene->AddElement(H, 8);
    
    // EJ-212 Scintillator material
    // Values from http://www.eljentechnology.com/index.php/joomla-overview/what-is-new-in-1-5/64-ej-212
    // and from http://www.eljentechnology.com/images/stories/Technical_Information/Physical%20Constant%20of%20Plastic%20Scintillators.pdf
    
    // Define the material and its composition
    EJ212 = new G4Material("EJ-212", density=1.023*g/cm3, ncomponents=2);
    EJ212->AddElement(H, 0.5243407708);
    EJ212->AddElement(C, 0.4756592292);
    
    
    // Add the material's optical properties
    const G4int n_ej212_entries = 26;
    
    // All the properties are stored as key/value pairs. The keys are photon
    // energies (not wavelengths!) the values are the property for that energy
    G4double ej212_photon_energies[n_ej212_entries] =
    {2.38*eV, 2.41*eV, 2.43*eV, 2.45*eV, 2.48*eV, 2.50*eV, 2.53*eV, 2.55*eV,
        2.58*eV, 2.61*eV, 2.64*eV, 2.66*eV, 2.69*eV, 2.72*eV, 2.75*eV, 2.78*eV,
        2.82*eV, 2.85*eV, 2.88*eV, 2.92*eV, 2.93*eV, 2.95*eV, 2.99*eV, 3.02*eV,
        3.06*eV, 3.10*eV};
    
    // For scintillation you define the relative amplitudes of each photon
    // energy in the emission spectrum. This is normalised to 1.
    G4double ej212_emission_spectrum [n_ej212_entries] =
    {0.005, 0.010, 0.020, 0.050, 0.095, 0.100, 0.100, 0.080, 0.070, 0.070,
        0.067, 0.055, 0.045, 0.036, 0.031, 0.027, 0.023, 0.020, 0.018, 0.016,
        0.014, 0.012, 0.010, 0.009, 0.008, 0.007};
    
    // Refractive index; this is static for most photon energies.
    G4double ej212_refractive_index [n_ej212_entries] =
        {1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
         1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58,
         1.58, 1.58, 1.58, 1.58};
    
    // Absorption/Attenuation length (distance for signal to drop to 1/e intial)
    G4double ej212_absorption_length [n_ej212_entries] =
       {250*cm, 250*cm, 250*cm, 250*cm, 250*cm, 250*cm, 250*cm, 250*cm, 250*cm,
        250*cm, 250*cm, 250*cm, 250*cm, 250*cm, 250*cm, 250*cm, 250*cm, 250*cm,
        250*cm, 250*cm, 250*cm, 250*cm, 250*cm, 250*cm, 250*cm, 250*cm};
    
    G4MaterialPropertiesTable* ej212_MPT = new G4MaterialPropertiesTable();
    ej212_MPT->AddProperty("RINDEX",                // The property name
                           ej212_photon_energies,   // photon energies (keys)
                           ej212_refractive_index,  // value for that photon energy
                           n_ej212_entries);        // number of key/value pairs
    
    // Add the scintillation emission spectrum (normalised to 1)
    ej212_MPT->AddProperty("FASTCOMPONENT",
                           ej212_photon_energies,
                           ej212_emission_spectrum,
                           n_ej212_entries);
    
    // Specify the absorption lengths
    ej212_MPT->AddProperty("ABSLENGTH",
                           ej212_photon_energies,
                           ej212_absorption_length,
                           n_ej212_entries);
    
    // How many photons / MeV / Step, this is assumed to be linear
    ej212_MPT->AddConstProperty("SCINTILLATIONYIELD", 10000.0/MeV);
    // Scale the variance of the number of photons / MeV / Step (e.g. due to
    // doping of the material and/or Fano factor)
    ej212_MPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
    // If EJ-212 had a
    ej212_MPT->AddConstProperty("YIELDRATIO", 1.0);
    // The time constant for the time evolution of the number of scintillation
    // photons (i.e. tau in the formula 'A*exp(-t/tau)' )
    ej212_MPT->AddConstProperty("FASTTIMECONSTANT", 2.4*ns);
    // There is also a 'FASTSCINTILLATIONRISETIME' that accounts for the rise
    // time of the scintillator but given that for EJ-212 this is 0.9ns
    // it will be ignored.
    
    // Set the material table
    EJ212->SetMaterialPropertiesTable(ej212_MPT);
    
    // BCF-91A Wavelength Shifting Fibre
    BCF91A_core = new G4Material("BCF-91A_core", density=1.05*g/cm3, ncomponents=2);
    BCF91A_core->AddElement(C, 0.5015511892);
    BCF91A_core->AddElement(H, 0.4984488108);
    
    // BCF-91A values are taken from
    // http://www-zeuthen.desy.de/lcdet/Feb_05_WS/talks/rd_lcdet_sim.pdf
    // Based on work by Hugh Gallager for MINOS (not locatable)
    const G4int n_bcf91a_core_absorption_photons = 42;
    // Specify the photon energies that we will be defining properties for
    G4double bcf91a_core_abs_photons[n_bcf91a_core_absorption_photons] =
    {3.539*eV, 3.477*eV, 3.340*eV, 3.321*eV, 3.291*eV, 3.214*eV, 3.162*eV,
        3.129*eV, 3.091*eV, 3.086*eV, 3.049*eV, 3.008*eV, 2.982*eV, 2.958*eV,
        2.928*eV, 2.905*eV, 2.895*eV, 2.890*eV, 2.858*eV, 2.813*eV, 2.774*eV,
        2.765*eV, 2.752*eV, 2.748*eV, 2.739*eV, 2.735*eV, 2.731*eV, 2.723*eV,
        2.719*eV, 2.698*eV, 2.674*eV, 2.626*eV, 2.610*eV, 2.583*eV, 2.556*eV,
        2.530*eV, 2.505*eV, 2.480*eV, 2.455*eV, 2.431*eV, 2.407*eV, 2.384*eV};
    // Absorptions are dealt with as lengths...
    G4double bcf91a_core_abs_lengths [n_bcf91a_core_absorption_photons] =
    { 0.28*cm,  0.28*cm,   0.26*cm,   0.25*cm,   0.24*cm,   0.21*cm,
        0.19*cm,  0.16*cm,   0.13*cm,   0.13*cm,   0.14*cm,   0.11*cm,
        0.08*cm,  0.05*cm,   0.02*cm,   0.05*cm,   0.08*cm,   0.10*cm,
        0.13*cm,  0.10*cm,   0.08*cm,   0.07*cm,   0.08*cm,   0.11*cm,
        0.13*cm,  0.16*cm,   0.19*cm,   0.21*cm,   0.24*cm,   0.27*cm,
        0.30*cm,  2.69*cm,   3.49*cm,   3.99*cm,   5.00*cm,  11.60*cm,
        21.60*cm, 33.10*cm, 175.00*cm, 393.00*cm, 617.00*cm, 794.00*cm};
    //todo convert absorption spectrum to lengths (hooray!)
    
    const G4int n_bcf91a_core_emission_photons = 24;
    G4double bcf91a_core_emission_photons [n_bcf91a_core_emission_photons] =
    {2.69*eV, 2.67*eV, 2.66*eV, 2.64*eV, 2.63*eV, 2.61*eV, 2.58*eV,
        2.56*eV, 2.55*eV, 2.53*eV, 2.50*eV, 2.48*eV, 2.46*eV, 2.45*eV,
        2.44*eV, 2.43*eV, 2.41*eV, 2.37*eV, 2.33*eV, 2.25*eV, 2.24*eV,
        2.19*eV, 2.15*eV, 2.08*eV};
    // Specify the normalised emission spectrum
    G4double bcf91a_core_emission_spectrum [n_bcf91a_core_emission_photons] =
    {0.00, 0.02, 0.09, 0.20, 0.29, 0.40, 0.59, 0.70, 0.80, 0.89, 1.00, 0.96,
        0.88, 0.79, 0.69, 0.59, 0.50, 0.40, 0.31, 0.22, 0.19, 0.10, 0.06, 0.00};
    
    // Refractive index. This is constant so set at extremes of
    // absorption/emission spectrum and the middle.
    const G4int n_bcf91a_core_rindex_photons = 3;
    G4double bcf91a_core_rindex_photons [n_bcf91a_core_rindex_photons] =
    {2.07*eV, 2.62*eV, 3.55*eV};
    G4double bcf91a_core_rindex [n_bcf91a_core_rindex_photons] =
    {1.60, 1.60, 1.60};
    
    G4MaterialPropertiesTable* bcf91a_core_MPT = new G4MaterialPropertiesTable();
    bcf91a_core_MPT->AddProperty("WLSABSLENGTH", // The WLS absorption spectrum
                                 bcf91a_core_abs_photons,
                                 bcf91a_core_abs_lengths,
                                 n_bcf91a_core_absorption_photons);
    bcf91a_core_MPT->AddProperty("WLSCOMPONENT", // WLS emission spectrum
                                 bcf91a_core_emission_photons,
                                 bcf91a_core_emission_spectrum,
                                 n_bcf91a_core_emission_photons);
    bcf91a_core_MPT->AddProperty("RINDEX",  // The core refractive index
                                 bcf91a_core_rindex_photons,
                                 bcf91a_core_rindex,
                                 n_bcf91a_core_rindex_photons);
    // Decay time
    bcf91a_core_MPT->AddConstProperty("WLSTIMECONSTANT", 12.0*ns);
    
    BCF91A_core->SetMaterialPropertiesTable(bcf91a_core_MPT);
    
    // Set up the material for the WLS Fibre's acrylic cladding. This is assumed
    // to be the same as the core except with a different refractive index
    // and not WLS or scintillating (absorption length set to very long)
    BCF91A_clad = new G4Material("BCF-91A_clad", density=1.05*g/cm3,ncomponents=3);
    // use the basic ratios of C:H:O, copied from g4 example:
    // 'advanced/air_shower/src/UltraDetectorConstruction.cc'
    BCF91A_clad->AddElement(C, 5);
    BCF91A_clad->AddElement(H, 8);
    BCF91A_clad->AddElement(O, 2);
    
    const G4int n_bcf91a_clad_photons = 3;
    G4double bcf91a_clad_photons[n_bcf91a_clad_photons] =
    {2.07*eV, 2.62*eV, 3.55*eV};
    // The refractive index is constant for wavelengths
    G4double bcf91a_clad_rindex[n_bcf91a_clad_photons] =
    {1.49, 1.49, 1.49};
    // Assume the absorption length of the cladding is constant and large
    G4double bcf91a_clad_abs_length[n_bcf91a_clad_photons] =
    {3.5*m, 3.5*m, 3.5*m};
    
    G4MaterialPropertiesTable* bcf91a_clad_MPT = new G4MaterialPropertiesTable();
    bcf91a_clad_MPT->AddProperty("RINDEX",
                                 bcf91a_clad_photons,
                                 bcf91a_clad_rindex,
                                 n_bcf91a_clad_photons);
    bcf91a_clad_MPT->AddProperty("ABSLENGTH",
                                 bcf91a_clad_photons,
                                 bcf91a_clad_abs_length,
                                 n_bcf91a_clad_photons);
    
    BCF91A_clad->SetMaterialPropertiesTable(bcf91a_clad_MPT);
    
// Create the Scintillator and WLS to Mylar optical surfaces
    mylar_surface = new G4OpticalSurface("scint_mylar_op_surface", // Name
                                          glisur,            // Which model to use
                                          polished,          // Surface type
                                          dielectric_metal,  // Interface type
                                          1.0);              // How polished
    
    const int n_mylar_photons = 2;
    double mylar_photons[n_mylar_photons]      = {2.0*eV, 3.6*eV};
    double mylar_reflectivity[n_mylar_photons] = {1., 1.};
    double mylar_efficiency[n_mylar_photons]   = {0.0, 0.0};
    
    G4MaterialPropertiesTable* mylar_MPT = new G4MaterialPropertiesTable();
    
    mylar_MPT->AddProperty("REFLECTIVITY",
                           mylar_photons, mylar_reflectivity, n_mylar_photons);
    mylar_MPT->AddProperty("EFFICIENCY",
                           mylar_photons, mylar_efficiency, n_mylar_photons);
    mylar_surface->SetMaterialPropertiesTable(mylar_MPT);
    // print table
    //
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}


// Utility function
G4ThreeVector get_global_pos(double dist_from_coil8)
{
    double coil8_x = (776.3)*mm;
    double coil8_z = (3420.1)*mm;
    double glb_x = dist_from_coil8*sin(36.0*deg) + coil8_x;
    double glb_z = dist_from_coil8*cos(36.0*deg) + coil8_z;
    return G4ThreeVector(glb_x,0.0,glb_z);
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // NB remember all solids are created using 'half lengths'
//-------------------------------------------------------------------------
// Instantiate the global magnetic field
//-------------------------------------------------------------------------
    
    if (f_myField)
    {
        delete f_myField;
    }
    
    if(!f_myField)
    {
        f_myField = new Field(f_fname_sol,f_fname_dip,f_dip_polarity);
        G4FieldManager* fieldMgr
        = G4TransportationManager::GetTransportationManager()
        ->GetFieldManager();
        fieldMgr->SetDetectorField(f_myField);
        fieldMgr->CreateChordFinder(f_myField);
    }
    
//-------------------------------------------------------------------------
// Global rotation matrix to position things correctly
//-------------------------------------------------------------------------
    G4RotationMatrix rotate_36 = G4RotationMatrix();
    rotate_36.rotateY(36.0*deg);
    
//-------------------------------------------------------------------------
// Specify the world geometry
//-------------------------------------------------------------------------
    double world_hx = (8000/2.0)*mm; // this is the max size of current field map
    double world_hy = (8000/2.0)*mm;
    double world_hz = (8000/2.0)*mm;
    G4Box * solid_world = new G4Box("world",world_hx,world_hy,world_hz);
    f_logic_world = new G4LogicalVolume(solid_world,Air,"world",0,0,0);
    f_physi_world = new G4PVPlacement(0,G4ThreeVector(),f_logic_world,"world", 0,false,0);
    
//-------------------------------------------------------------------------
// Set the variables for defining the geometry size
//-------------------------------------------------------------------------
    
    // Position of upstream scintillator WRT coil8. This is the basic
    // offset from which everything else is measured.
    const double coil8_to_u_scint = 440*mm;
    
// Scintillator measurements
    // Numbers of each scintillator
    const int n_u_scints = 8;
    const int n_d_scints = 5;
    // Width of the scintillators is common to both up and down stream
    const double scint_x   = 380.0*mm/2.0;
    // Smaller upstream scintillators
    const double u_scint_y =  30.0*mm/2.0;
    const double u_scint_z =   0.5*mm/2.0;
    // Larger downstream scintillators
    const double d_scint_y =  50.0*mm/2.0;
    const double d_scint_z =   3.5*mm/2.0;

// Target measurements
    const double target_x = 370.0*mm/2.0;
    const double target_y = 310.0*mm/2.0;

// Degrader measurements
    const double degrader_x = 400.0*mm/2.0;
    const double degrader_y = 400.0*mm/2.0;
    
// WLS fibre measurements
    const double wls_length     = 500.0*mm/2.0;
    const double wls_radius     =   0.5*mm;
    // cladding is specified to be 3% of the fibre's diameter
    const double wls_cladding_r = 0.03 * 2.0 * wls_radius;
    // To attach the WLS to the scintillator 90 degrees of a tube are
    // implemented as scintillator to mimic optical cement, this is the
    // outer radius of this layer.
    const double optical_cement_radius = 0.8*mm;
// MPPC measurements (implemented as an Al cube)
    const double mppc_size = 1.0*mm/2.0;
    
// Misc offsets
    // Mylar cladding
    // This is not actually instantiated as a volume but as a G4OpticalSurface
    // want the thickness for offsets
    const double mylar_thickness = 2*um/2.0;
    // Thickness of the aluminium frame that separates the scint, degrader and
    // target. Not actually created, just used for offsets
    const double al_dz = 3.0*mm;

//-------------------------------------------------------------------------
// Specify the common volumes (WLS, MPPC, optical cement)
//-------------------------------------------------------------------------
    // The WLS is the same either up or downstream
    // Core of the WLS (the bit that actually wavelength shifts
    G4Tubs* wls_fibre_core = new G4Tubs("wls_fibre_core_solid", // Name
                                        0.0,              // Not hollow
                                        wls_radius - wls_cladding_r, // Allow for cladding
                                        wls_length,       // Length
                                        0.0,              // Start is irrelevant
                                        360*degree);      // Complete circle
    G4LogicalVolume* wls_fibre_core_log =
            new G4LogicalVolume(wls_fibre_core, BCF91A_core, "wls_fibre_core_log");
    
    // Make the WLS fibre's cladding
    G4Tubs* wls_fibre_clad = new G4Tubs("wls_fibre_cladding_solid",  // Name
                                        wls_radius - wls_cladding_r, // Wrap the core
                                        wls_radius,          // Full radius
                                        wls_length,          // Length
                                        0.0,                 // Start is irrelevant
                                        360*degree);         // Complete circle
    G4LogicalVolume* wls_fibre_clad_log =
            new G4LogicalVolume(wls_fibre_clad, BCF91A_clad, "wls_fibre_clad_log");
    
    // The MPPC
    G4Box* mppc_solid = new G4Box("mppc_solid", mppc_size, mppc_size, mppc_size);
    // Make the MPPC out of air to prevent dielectric/metal interfaces
    G4LogicalVolume* mppc_log =
            new G4LogicalVolume(mppc_solid, Air, "mppc_log");
    
    // Set up the (complex) G4 solids i.e. the scintillator/WLS pairs
    // Create the grove that will hold the WLS that represents the optical cement
    G4Tubs* optical_cement_tubs = new G4Tubs("optical_cement_tubs", // Name
                                             // Inner radius, matched to WLS.
                                             wls_radius,
                                             // Outer radius, to form WLS 'trough'
                                             optical_cement_radius,
                                             // Extrusion along z (will be rotated to x)
                                             scint_x,
                                             -45.0*degree,    // Starting angle
                                             90.0*degree);    // Opening angle
    
//-------------------------------------------------------------------------
// Create the upstream scintillator
//-------------------------------------------------------------------------
    // The specific solid for the scintillator
    G4Box* u_scint_box = new G4Box("u_scint_box", scint_x, u_scint_y, u_scint_z);
    
    // Create the transformation to form the scintillator/WLS grove
    //
    // Rotate the 'optical cement' around the Y axis to lie perpendicular to
    // the beam direction   
    G4RotationMatrix rotate_tub_90;
    rotate_tub_90.rotateY(90*degree);
    // Move the cement from behind the scintillator to just in front of it
    G4ThreeVector u_scint_translate(0.0, 0.0, wls_radius + u_scint_z);
    // Create the total 3D transform: rotation and translation
    G4Transform3D u_scint_transform(rotate_tub_90, u_scint_translate);
    
    // Create the combined bar and WLS grove
    G4UnionSolid* u_scint_solid = new G4UnionSolid("u_scint_solid",    // Name
                                                   u_scint_box,        // Solid1
                                                   optical_cement_tubs,// Solid2
                                                   u_scint_transform); // How to add them
    
    // Create the logical volumes that we will add to the assembly volume
    G4LogicalVolume* u_scint_log =
            new G4LogicalVolume(u_scint_solid, EJ212, "u_scint_log");
    
    // Create the assembly that will hold all the 'bits'
    G4AssemblyVolume* u_assembly = new G4AssemblyVolume();
    G4ThreeVector translate(0.0, 0.0, 0.0);
    const G4RotationMatrix no_rotation;
    G4Transform3D transform (no_rotation, translate);
    u_assembly->AddPlacedVolume(u_scint_log, transform);
    
    // Create the 3D transform for the wls fibre. They use the same translation
    // as they are nested one in the other
    translate = G4ThreeVector(0.0, 0.0, wls_radius+u_scint_z);
    // Rotate the WLS through 90 degrees so normal to beam direction
    transform = G4Transform3D(rotate_tub_90, translate);
    u_assembly->AddPlacedVolume(wls_fibre_clad_log, transform);
    u_assembly->AddPlacedVolume(wls_fibre_core_log, transform);
    
    // Create the transform for the MPPCs so they sit on the ends of the WLS
    translate = G4ThreeVector( 1.0*(wls_length+mppc_size), 0.0, wls_radius+u_scint_z);
    transform = G4Transform3D(no_rotation, translate);
    u_assembly->AddPlacedVolume(mppc_log, transform);
    // second MPPC
    translate = G4ThreeVector(-1.0*(wls_length+mppc_size), 0.0, wls_radius+u_scint_z);
    transform = G4Transform3D(no_rotation, translate);
    u_assembly->AddPlacedVolume(mppc_log, transform);
    
    // Constants that are useful in positioning
    // Step size is one scint height + the mylar thickness (want air gap
    // to implement the surface correctly).
    const double u_y_step   = 2.0 * (u_scint_y + mylar_thickness);
    // Start from the bottom
    const double u_y_offset = 0.5 * u_y_step * static_cast<double>(n_u_scints-1);
    
    // Use `1.1` to dodge floating point issues
    for (double y = -1.0*u_y_offset; y < 1.1*u_y_offset; y += u_y_step)
    {
        G4ThreeVector u_place_translate = get_global_pos(coil8_to_u_scint);
        u_place_translate += G4ThreeVector(0.0, y, 0.0);
        G4Transform3D u_place_transform (rotate_36, u_place_translate);
        u_assembly->MakeImprint(f_logic_world, u_place_transform);
    }
    
    // Now loop over all the placed volumes and, where needed, specify
    // optical surfaces i.e. add the mylar wrap
    typedef std::vector<G4VPhysicalVolume*>::iterator assembly_iterator;
    assembly_iterator u_it  = u_assembly->GetVolumesIterator();
    assembly_iterator u_end = u_assembly->GetVolumesIterator() + u_assembly->TotalImprintedVolumes();
    for (; u_it < u_end; u_it++)
    {
        // Only the scintillator and the _cladding_ of the WLS are wrapped in
        // mylar so find those. Test against logical volumes as placements
        // have mangled names (due to placement process).
        if ((*u_it)->GetLogicalVolume()->GetName() == "u_scint_log" ||
            (*u_it)->GetLogicalVolume()->GetName() == "wls_fibre_clad_log")
        {
            new G4LogicalBorderSurface("mylar_wrap",  // Surface name
                                       (*u_it),         // Leaving vol 1
                                       f_physi_world, // Entering vol 2
                                       mylar_surface);// Has this surface
        }
    }
    
//-------------------------------------------------------------------------
// Create the target
//-------------------------------------------------------------------------
    
    // We'll use this distance later
    const double target_position_z = coil8_to_u_scint + u_scint_z + al_dz + f_targetZ;
    G4ThreeVector target_pos = get_global_pos(target_position_z);
    G4Box* solid_target = new G4Box("target", target_x, target_y, f_targetZ);
    G4LogicalVolume* target_log = new G4LogicalVolume(solid_target,f_targetMat,"target",0,0,0);
    G4VPhysicalVolume* target_phys = new G4PVPlacement(G4Transform3D(rotate_36,target_pos),
                                                       target_log,"target",
                                                       f_logic_world,false,0);

//-------------------------------------------------------------------------
// Create the downstream scintillator
//-------------------------------------------------------------------------
    // We can re-use some of the variables from the upstream scintillator
    G4Box* d_scint_box = new G4Box("d_scint_box", scint_x, d_scint_y, d_scint_z);
    G4ThreeVector d_scint_translate(0.0, 0.0, wls_radius + d_scint_z);
    // The rotation is the same as previously
    G4Transform3D d_scint_transform(rotate_tub_90, d_scint_translate);
    
    G4UnionSolid* d_scint_solid = new G4UnionSolid("d_scint_solid",    // Name
                                                   d_scint_box,        // Solid1
                                                   optical_cement_tubs,// Solid2
                                                   d_scint_transform); // Transform
    // Create the logical volume
    G4LogicalVolume* d_scint_log
            = new G4LogicalVolume(d_scint_solid, EJ212, "d_scint_log");
    
    G4AssemblyVolume* d_assembly = new G4AssemblyVolume();
    // Reuse the u's transformations etc.
    translate = G4ThreeVector(0.0, 0.0, 0.0); // Reset to zero for the bar
    transform = G4Transform3D(no_rotation, translate);
    d_assembly->AddPlacedVolume(d_scint_log, transform);
    
    // Add the WLS core & cladding
    translate = G4ThreeVector(0.0, 0.0, wls_radius+d_scint_z);
    transform = G4Transform3D(rotate_tub_90, translate);
    d_assembly->AddPlacedVolume(wls_fibre_clad_log, transform);
    d_assembly->AddPlacedVolume(wls_fibre_core_log, transform);
    
    // Add the MPPC
    translate = G4ThreeVector( 1.0*(wls_length+mppc_size), 0.0, wls_radius+d_scint_z);
    transform = G4Transform3D(no_rotation, translate);
    d_assembly->AddPlacedVolume(mppc_log, transform);
    
    translate = G4ThreeVector(-1.0*(wls_length+mppc_size), 0.0, wls_radius+d_scint_z);
    transform = G4Transform3D(no_rotation, translate);
    d_assembly->AddPlacedVolume(mppc_log, transform);
    
    // Y displacement step size
    const double d_y_step = 2.0 * (d_scint_y + mylar_thickness);
    // Y displacement offset
    const double d_y_offset = 0.5 * d_y_step * static_cast<double>(n_d_scints-1);
    // position the scintillator relative to the centre of the target
    const double d_position = target_position_z + f_targetZ + al_dz + d_scint_z;
    // Place the d scintillators
    for (double y = -1.0*d_y_offset; y < 1.1*d_y_offset; y += d_y_step)
    {
        G4ThreeVector d_place_translate = get_global_pos(d_position);
        d_place_translate += G4ThreeVector(0.0, y, 0.0);
        G4Transform3D d_place_transform (rotate_36, d_place_translate);
        d_assembly->MakeImprint(f_logic_world, d_place_transform);
    }
    
    
    // Now loop over all the placed volumes and, where needed, specify
    // optical surfaces i.e. add the mylar wrap
    assembly_iterator d_it  = d_assembly->GetVolumesIterator();
    assembly_iterator d_end = d_assembly->GetVolumesIterator() + d_assembly->TotalImprintedVolumes();
    for (; d_it < d_end; d_it++)
    {
        // Only the scintillator and the _cladding_ of the WLS are wrapped in
        // mylar so find those. Test against logical volumes as placements
        // have mangled names (due to placement process).
        if ((*d_it)->GetLogicalVolume()->GetName() == "d_scint_log" ||
            (*d_it)->GetLogicalVolume()->GetName() == "wls_fibre_clad_log")
        {
            new G4LogicalBorderSurface("mylar_wrap",  // Surface name
                                       (*d_it),       // Leaving vol 1
                                       f_physi_world, // Entering vol 2
                                       mylar_surface);// Has this surface
        }
    }
    
//-------------------------------------------------------------------------
// Create the degrader
//-------------------------------------------------------------------------
    
    // degrader is in front of scint 1
    double degrader_posz = coil8_to_u_scint - u_scint_z - al_dz - f_degraderZ;
    G4ThreeVector degrader_pos = get_global_pos(degrader_posz);
    G4Box* degrader_solid = new G4Box("degrader", degrader_x, degrader_y, f_degraderZ);
    G4LogicalVolume* degrader_log =
        new G4LogicalVolume(degrader_solid,f_degraderMat,"degrader",0,0,0);
    G4PVPlacement* degrader_phys =
        new G4PVPlacement(G4Transform3D(rotate_36,degrader_pos),
                          degrader_log,"degrader", f_logic_world,false,0);
    
    // the limits on step size in the interesting volumes to be 1/10 z
    double u_scint_limit = 0.1 * u_scint_z;
    double d_scint_limit = 0.1 * d_scint_z;
    double st_limit  = 0.1 * f_targetZ;
    double deg_limit = 0.1 * f_degraderZ;
    
    f_u_limit = new G4UserLimits(u_scint_limit);
    u_scint_log->SetUserLimits(f_u_limit);
    
    f_d_limit = new G4UserLimits(d_scint_limit);
    d_scint_log->SetUserLimits(f_d_limit);
    
    f_st_limit = new G4UserLimits(st_limit);
    target_log->SetUserLimits(f_st_limit);
    
    f_deg_limit = new G4UserLimits(deg_limit);
    degrader_log->SetUserLimits(f_deg_limit);
    
    
    return f_physi_world;
}

void DetectorConstruction::SetPolarity(double newPol)
{
    if (newPol > 1.0)
    {
        G4cerr << newPol << " polarity value too large, must be between -1 and 1. Set to 1" << G4endl;
        f_dip_polarity = 1;
    } else if (newPol < -1.0)
    {
        G4cerr << newPol << " polarity value too small, must be between -1 and 1. Set to -1" << G4endl;
        f_dip_polarity = -1;
    } else {
        f_dip_polarity = newPol;
    }
    f_updated = true;
}

void DetectorConstruction::SetDegraderMat(G4String newVal)
{
    G4Material* newMat = G4Material::GetMaterial(newVal);
    if (newMat)
    {
        f_degraderMat = newMat;
        //        f_logic_degrader->SetMaterial(f_degraderMat);
        G4cout << "Degrader now made from "<< newVal << G4endl;
    }
    f_updated = true;
}
void DetectorConstruction::SetTargetMat(G4String newVal)
{
    G4Material* newMat = G4Material::GetMaterial(newVal);
    if (newMat)
    {
        f_targetMat = newMat;
        //        f_logic_target->SetMaterial(f_targetMat);
        G4cout << "Target now made from "<< newVal << G4endl;
    }
    f_updated = true;
}

void DetectorConstruction::SetDipFieldName(G4String newFile)
{
    f_fname_dip = newFile;
    //    f_myField->SetDipoleField(newFile);
    f_updated = true;
}

void DetectorConstruction::SetSolFieldName(G4String newFile)
{
    f_fname_sol = newFile;
    //    f_myField->SetSolenoidField(newFile);
    f_updated=true;
}

void DetectorConstruction::UpdateGeometry()
{
    
    // clean-up previous geometry
    G4GeometryManager::GetInstance()->OpenGeometry();
    
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    
    //define new one
    G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
    
    f_updated=false;
}
