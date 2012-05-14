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
#include "G4VisAttributes.hh"
#include "G4Colour.hh"


#include <string.h>

DetectorConstruction::DetectorConstruction(
      G4String fname_sol,
      G4String fname_dip,
      int polarity)
{

   f_fname_sol = fname_sol;
   f_fname_dip = fname_dip;
   f_dip_polarity = polarity;

//#include "DetectorParameterDef.icc"
  DefineMaterials();

}

DetectorConstruction::~DetectorConstruction()
{
  //delete Scinti;
  //delete Silicon;
  //delete Ar;
  //delete Lead;
  //delete Air;

  //delete O;
  //delete N;
  //delete C;
  //delete H;
}

/*
void DetectorConstruction::DefineMaterials()
{
  //-------------------------------------------------------------------------
  // Materials
  //-------------------------------------------------------------------------

  G4double a, z, density;
  G4int nel;

  H = new G4Element("Hydrogen", "H", z=1., a=  1.01*g/mole);
  C = new G4Element("Carbon",   "C", z=6., a= 12.01*g/mole);
  N = new G4Element("Nitrogen", "N", z=7., a= 14.01*g/mole);
  O = new G4Element("Oxygen",   "O", z=8., a= 16.00*g/mole);

  Air = new G4Material("Air", density= 1.29*mg/cm3, nel=2);
  Air->AddElement(N, 70.*perCent);
  Air->AddElement(O, 30.*perCent);

  Lead = 
  new G4Material("Lead", z=82., a= 207.19*g/mole, density= 11.35*g/cm3);

  Ar = 
  new G4Material("ArgonGas",z=18., a= 39.95*g/mole, density=1.782*mg/cm3);

  Silicon = 
  new G4Material("Silicon", z=14., a= 28.09*g/mole, density= 2.33*g/cm3);

  Scinti = new G4Material("Scintillator", density= 1.032*g/cm3, nel=2);
  Scinti->AddElement(C, 9);
  Scinti->AddElement(H, 10);
}
*/

void DetectorConstruction::DefineMaterials()
{ 
 //This function illustrates the possible ways to define materials
 
G4String symbol;             //a=mass of a mole;
G4double a, z, density;      //z=mean number of protons;  
G4int iz, n;                 //iz=number of protons  in an isotope; 
                             // n=number of nucleons in an isotope;

G4int ncomponents, natoms;
G4double abundance, fractionmass;

//
// define Elements
//

G4Element* H  = new G4Element("Hydrogen",symbol="H" , z= 1., a= 1.01*g/mole);
G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z= 6., a= 12.01*g/mole);
G4Element* N  = new G4Element("Nitrogen",symbol="N" , z= 7., a= 14.01*g/mole);
G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
G4Element* Si = new G4Element("Silicon",symbol="Si" , z= 14., a= 28.09*g/mole);

//
// define an Element from isotopes, by relative abundance 
//

G4Isotope* U5 = new G4Isotope("U235", iz=92, n=235, a=235.01*g/mole);
G4Isotope* U8 = new G4Isotope("U238", iz=92, n=238, a=238.03*g/mole);

G4Element* U  = new G4Element("enriched Uranium",symbol="U",ncomponents=2);
U->AddIsotope(U5, abundance= 90.*perCent);
U->AddIsotope(U8, abundance= 10.*perCent);

//
// define simple materials
//

Al = new G4Material("Aluminium", z=13., a=26.98*g/mole, density=2.700*g/cm3);
new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
new G4Material("Lead"     , z=82., a= 207.19*g/mole, density= 11.35*g/cm3);
G4Material* Mg = new G4Material("Magnesium", z=12., a= 24.305*g/mole, density= 1.738*g/cm3);
G4Material* Cu = new G4Material("Copper", z=29., a= 63.546*g/mole, density= 8.94*g/cm3);

//
// define a material from elements.   case 1: chemical molecule
//

G4Material* H2O = 
new G4Material("Water", density= 1.000*g/cm3, ncomponents=2);
H2O->AddElement(H, natoms=2);
H2O->AddElement(O, natoms=1);
// overwrite computed meanExcitationEnergy with ICRU recommended value 
H2O->GetIonisation()->SetMeanExcitationEnergy(75.0*eV);

G4Material* Sci = 
new G4Material("Scintillator", density= 1.032*g/cm3, ncomponents=2);
Sci->AddElement(C, natoms=9);
Sci->AddElement(H, natoms=10);

G4Material* Myl = 
new G4Material("Mylar", density= 1.397*g/cm3, ncomponents=3);
Myl->AddElement(C, natoms=10);
Myl->AddElement(H, natoms= 8);
Myl->AddElement(O, natoms= 4);

G4Material* SiO2 = 
new G4Material("quartz",density= 2.200*g/cm3, ncomponents=2);
SiO2->AddElement(Si, natoms=1);
SiO2->AddElement(O , natoms=2);

//
// define a material from elements.   case 2: mixture by fractional mass
//

G4Material* Air = 
new G4Material("Air"  , density= 1.290*mg/cm3, ncomponents=2);
Air->AddElement(N, fractionmass=0.7);
Air->AddElement(O, fractionmass=0.3);

//
// define a material from elements and/or others materials (mixture of mixtures)
//

G4Material* Aerog = 
new G4Material("Aerogel", density= 0.200*g/cm3, ncomponents=3);
Aerog->AddMaterial(SiO2, fractionmass=62.5*perCent);
Aerog->AddMaterial(H2O , fractionmass=37.4*perCent);
Aerog->AddElement (C   , fractionmass= 0.1*perCent);

//
// examples of gas in non STP conditions
//

G4Material* CO2 = 
new G4Material("CarbonicGas", density= 1.842*mg/cm3, ncomponents=2,
                              kStateGas, 325.*kelvin, 50.*atmosphere);
CO2->AddElement(C, natoms=1);
CO2->AddElement(O, natoms=2);
 
G4Material* steam = 
new G4Material("WaterSteam", density= 0.3*mg/cm3, ncomponents=1,
                             kStateGas, 500.*kelvin, 2.*atmosphere);
steam->AddMaterial(H2O, fractionmass=1.);

//
// examples of vacuum
//

G4Material* Vacuum =
new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                           kStateGas, 2.73*kelvin, 3.e-18*pascal);

G4Material* beam = 
new G4Material("Beam", density= 1.e-5*g/cm3, ncomponents=1,
                       kStateGas, STP_Temperature, 2.e-2*bar);
beam->AddMaterial(Air, fractionmass=1.);

// Mylar
Mylar = new G4Material("Mylar", density= 1.397*g/cm3, ncomponents=3);
Mylar->AddElement(C,natoms=10);
Mylar->AddElement(H,natoms=8);
Mylar->AddElement(O,natoms=4);

   //Polystyrene
   Polystyrene = new G4Material("Polystyrene", density= 1.03*g/cm3, 2);
   Polystyrene->AddElement(C, 8);
   Polystyrene->AddElement(H, 8);
//
// or use G4-NIST materials data base
//
//G4NistManager* man = G4NistManager::Instance();
//man->FindOrBuildMaterial("G4_SODIUM_IODIDE");

// print table
//
G4cout << *(G4Material::GetMaterialTable()) << G4endl;

//default materials of the World
//defaultMaterial  = Vacuum;
matVac = Vacuum;
matAir = Air;
matSci = Sci;
matCu = Cu;
matMg = Mg;
matMylar = Mylar;
matDegrader = Polystyrene;
}

G4ThreeVector get_global_pos(double dist_from_coil8)
{
   double coil8_x = 776.3;
   double coil8_z = 3420.1;
   double glb_x = dist_from_coil8*sin(36.0*deg) + coil8_x;
   double glb_z = dist_from_coil8*cos(36.0*deg) + coil8_z;
   return G4ThreeVector(glb_x,0.0,glb_z);
}
G4VPhysicalVolume* DetectorConstruction::Construct()
{
   //-------------------------------------------------------------------------
   // Magnetic field
   //-------------------------------------------------------------------------

#if 1
   static G4bool fieldIsInitialized = false;
   if(!fieldIsInitialized)
   {
      Field* myField = new Field(f_fname_sol,f_fname_dip,f_dip_polarity);
      G4FieldManager* fieldMgr
         = G4TransportationManager::GetTransportationManager()
         ->GetFieldManager();
      fieldMgr->SetDetectorField(myField);
      fieldMgr->CreateChordFinder(myField);
      fieldIsInitialized = true;
   }
#endif

   //-------------------------------------------------------------------------
   // Rotation Matrix
   //-------------------------------------------------------------------------
   G4RotationMatrix rot_36 = G4RotationMatrix();
   rot_36.rotateY(36.0*deg);


   //-------------------------------------------------------------------------
   // Detector Geometry
   //-------------------------------------------------------------------------

   /* world */
   double world_hx = 8000/2.0; // 10m length
   double world_hy = 8000/2.0;
   double world_hz = 8000/2.0;
   G4Box * solid_world = new G4Box("world",world_hx,world_hy,world_hz);
   G4LogicalVolume * logic_world = new G4LogicalVolume(solid_world,matAir,"world",0,0,0);
   G4VPhysicalVolume * physi_world = new G4PVPlacement(0,G4ThreeVector(),logic_world,"world", 0,false,0);

   /* scintillator bar (sci1) */
   double sci1_hx = 380.0/2.0;
   double sci1_hy = 50.0/2.0;
   double sci1_hz = 3.5/2.0;
   G4ThreeVector sci1_pos = get_global_pos(439.75);
   G4Box* solid_sci1 = new G4Box("sci1",sci1_hx,sci1_hy,sci1_hz);
   G4LogicalVolume* logic_sci1 = new G4LogicalVolume(solid_sci1,matSci,"sci1",0,0,0);
   G4VPhysicalVolume* physi_sci1 = new G4PVPlacement(G4Transform3D(rot_36,sci1_pos),logic_sci1,"sci1", logic_world,false,0);
    
    double deg_hx = 380.0/2.0;
    double deg_hy = 50.0/2.0;
    double deg_hz = 1/2.0;
    G4ThreeVector deg_pos = get_global_pos(435.75);
    G4Box* solid_deg = new G4Box("sci1",deg_hx,deg_hy,deg_hz);
    G4LogicalVolume* logic_deg = new G4LogicalVolume(solid_deg,Polystyrene,"deg",0,0,0);
    G4VPhysicalVolume* physi_deg = new G4PVPlacement(G4Transform3D(rot_36,deg_pos),logic_deg,"deg", logic_world,false,0);
    
   /* scintillator bar (sci2) */
   double sci2_hx = sci1_hx;
   double sci2_hy = sci1_hy;
   double sci2_hz = sci1_hz;
   G4ThreeVector sci2_pos = get_global_pos(455.25);
   G4Box* solid_sci2 = new G4Box("sci2",sci2_hx,sci2_hy,sci2_hz);
   G4LogicalVolume* logic_sci2 = new G4LogicalVolume(solid_sci2,matSci,"sci2",0,0,0);
   G4VPhysicalVolume* physi_sci2 = new G4PVPlacement(G4Transform3D(rot_36,sci2_pos),logic_sci2,"sci2", logic_world,false,0);

   /* copper target (target) */
   double target_hx = 370.0/2.0;
   double target_hy = 80.0/2.0;
   double target_hz = 6.0/2.0;
   G4ThreeVector target_pos = get_global_pos(447.5);
   G4Box* solid_target = new G4Box("target",target_hx,target_hy,target_hz);
   G4LogicalVolume* logic_target = new G4LogicalVolume(solid_target,matCu,"target",0,0,0);
   G4VPhysicalVolume* physi_target = new G4PVPlacement(G4Transform3D(rot_36,target_pos),logic_target,"target", logic_world,false,0);


#if 0
   /* Magnetic field region */
   //double field_hx = (1500-500)*mm/4.0;  // 250mm
   //double field_hy = 50*cm/2.0;          // 250mm
   //double field_hz = (4288-3420)*mm/4.0; // 217mm
   double field_hx = 500*mm/2.0;
   double field_hy = 500*mm/2.0;
   double field_hz = 500*mm/2.0;
   double field_x = (1500+500)*mm/2.0;   // 1000mm
   double field_y = 0.0;
   double field_z = (4288+3420-2.0*100)*mm/2.0; // 3754mm
   solid_field = new G4Box("field",field_hx,field_hy,field_hz);
   logic_field = new G4LogicalVolume(solid_field,matAir,"field",0,0,0);
   physi_field = new G4PVPlacement(0,G4ThreeVector(field_x,field_y,field_z),logic_field,"field", logic_world,false,0);
#endif

   //-------------------------------------------------------------------------
   // Visual Attributes
   //-------------------------------------------------------------------------
   G4VisAttributes* colR = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
   G4VisAttributes* colG = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
   G4VisAttributes* colB = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
   G4VisAttributes* colLB = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
   logic_world->SetVisAttributes(G4VisAttributes::GetInvisible());

   logic_sci1->SetVisAttributes(colR);
   logic_sci2->SetVisAttributes(colG);
   logic_target->SetVisAttributes(colB);
   //logic_field->SetVisAttributes(colB);

   return physi_world;
}

