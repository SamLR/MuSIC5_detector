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

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4SDManager.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"
#include "G4RunManager.hh"
#include "G4UserLimits.hh"


#include <string.h>

DetectorConstruction::DetectorConstruction()
:f_myField(0), f_updated(false),
f_fname_sol("../../fieldmap/MuSIC5_detector/fieldmap_solenoid.txt"), 
f_fname_dip("../../fieldmap/MuSIC5_detector/fieldmap_dipole.txt"),
f_dip_polarity(1.0),
f_scint1z(0.5*mm), f_scint2z (3.5*mm), f_degraderZ(1*mm), f_targetZ(6*mm),
f_scint1_limit (0), f_scint2_limit(0), f_deg_limit(0), f_st_limit(0) {
        
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
    delete f_scint1_limit;
    delete f_scint2_limit;
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
// print table
//
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

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
   //-------------------------------------------------------------------------
   // Magnetic field
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
   // Rotation Matrix
   //-------------------------------------------------------------------------
    G4RotationMatrix rot_36 = G4RotationMatrix();
    rot_36.rotateY(36.0*deg);


   //-------------------------------------------------------------------------
   // Detector Geometry
   //-------------------------------------------------------------------------
   // world 
    double world_hx = (8000/2.0)*mm; // this is the max size of current field map
    double world_hy = (8000/2.0)*mm;
    double world_hz = (8000/2.0)*mm;
    G4Box * solid_world = new G4Box("world",world_hx,world_hy,world_hz);
    f_logic_world = new G4LogicalVolume(solid_world,Air,"world",0,0,0);
    f_physi_world = new G4PVPlacement(0,G4ThreeVector(),f_logic_world,"world", 0,false,0);

    // position of scint1 WRT coil8
    double coil8_to_scint1 = 440*mm; 

    double sci_hx = (380.0/2.0)*mm;  // both scintillators are the same width
    double sci1_hy = (240.0/2.0)*mm; // upstream counter 8x30mm strips
    double sci2_hy = (250.0/2.0)*mm; // Downstream counter is 5x50mm strips
    G4ThreeVector sci1_pos = get_global_pos(coil8_to_scint1);
    G4Box* solid_sci1 = new G4Box("sci1", sci_hx, sci1_hy, f_scint1z/2.0);
    f_logic_sci1 = new G4LogicalVolume(solid_sci1,f_scint1Mat,"sci1",0,0,0);
    f_physi_sci1 = new G4PVPlacement(G4Transform3D(rot_36,sci1_pos),f_logic_sci1,"sci1", f_logic_world,false,0);

    //copper target (target) 
    double target_posZ = coil8_to_scint1 + (f_scint1z/2.0) + 3 + (f_targetZ/2.0); // '3' is thickness of separators
    double target_hx = (450.0/2.0)*mm;
    double target_hy = (450.0/2.0)*mm;
    G4ThreeVector target_pos = get_global_pos(target_posZ);
    G4Box* solid_target = new G4Box("target",target_hx,target_hy,f_targetZ/2.0);
    f_logic_target = new G4LogicalVolume(solid_target,f_targetMat,"target",0,0,0);
    f_physi_target = new G4PVPlacement(G4Transform3D(rot_36,target_pos),f_logic_target,"target", f_logic_world,false,0);

   // scintillator bar (sci2)
   // dimensions are the same as scint 1 (other than thickness)
    double scint2_posz = target_posZ + 3 + (f_scint2z/2.0);
    G4ThreeVector sci2_pos = get_global_pos(scint2_posz);
    G4Box* solid_sci2 = new G4Box("sci2", sci_hx, sci2_hy, f_scint2z/2.0);
    f_logic_sci2 = new G4LogicalVolume(solid_sci2,f_scint2Mat,"sci2",0,0,0);
    
    f_physi_sci2 = new G4PVPlacement(G4Transform3D(rot_36,sci2_pos),f_logic_sci2,"sci2", f_logic_world,false,0);

    // degrader
    // degrader is in front of scint 1
    double degrader_posz = coil8_to_scint1 - (f_scint1z/2.0) - 3 - (f_degraderZ/2.0);
    double degrader_hx = (450.0/2.0)*mm;
    double degrader_hy = (450.0/2.0)*mm;
    G4ThreeVector degrader_pos = get_global_pos(degrader_posz);
    G4Box* solid_degrader = new G4Box("degrader",degrader_hx,degrader_hy,f_degraderZ/2.0);
    f_logic_degrader = new G4LogicalVolume(solid_degrader,f_degraderMat,"degrader",0,0,0);
    f_physi_degrader = new G4PVPlacement(G4Transform3D(rot_36,degrader_pos),f_logic_degrader,"degrader", f_logic_world,false,0);
    
// the limits on step size in the interesting volumes to be 1/10 z
    double scint1_limit = 0.1 * f_scint1z;
    double scint2_limit = 0.1 * f_scint2z;
    double st_limit = 0.1 * f_targetZ;
    double deg_limit = 0.1 * f_degraderZ;
    
    f_scint1_limit = new G4UserLimits(scint1_limit);
    f_logic_sci1->SetUserLimits(f_scint1_limit);
    
    f_scint2_limit = new G4UserLimits(scint2_limit);
    f_logic_sci2->SetUserLimits(f_scint2_limit);
    
    f_st_limit = new G4UserLimits(st_limit);
    f_logic_target->SetUserLimits(f_st_limit);
    
    f_deg_limit = new G4UserLimits(deg_limit);
    f_logic_degrader->SetUserLimits(f_deg_limit);
    

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
