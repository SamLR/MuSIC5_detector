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
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"
#include "G4RunManager.hh"


#include <string.h>

DetectorConstruction::DetectorConstruction()
    :f_myField(0), f_updated(false),
    f_fname_sol("fieldmap/fieldmap_solenoid.txt"), f_fname_dip("fieldmap/fieldmap_dipole.txt"),
    f_dip_polarity(1.0),
    f_scint1z(3.5*mm), f_scint2z (3.5*mm), f_degraderZ(0.2*mm), f_targetZ(6*mm) 
{
    f_messenger = new DetectorConstructionMessenger(this);
    DefineMaterials();
    f_degraderMat = G4Material::GetMaterial("Polystyrene");
    f_targetMat = G4Material::GetMaterial("Copper");
}

DetectorConstruction::~DetectorConstruction()
{
    delete f_myField;

    delete f_logic_world; // world volume
    delete f_physi_world;
    delete f_logic_sci1; // scint 1 & 2
    delete f_physi_sci1;
    delete f_logic_sci2;
    delete f_physi_sci2;
    delete f_logic_target; // target
    delete f_physi_target;
    delete f_logic_degrader; // degrader
    delete f_physi_degrader;
    
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
// elements for use as is
    Al = new G4Material("Aluminium", z=13.0, a= 26.98 *g/mole, density= 2.700*g/cm3);
    Pb = new G4Material("Lead",      z=82.0, a= 207.19*g/mole, density= 11.35*g/cm3);
    Mg = new G4Material("Magnesium", z=12.0, a= 24.305*g/mole, density= 1.738*g/cm3);
    Cu = new G4Material("Copper",    z=29.0, a= 63.546*g/mole, density= 8.94 *g/cm3);

// mixtures and compounds
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
// print table
//
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
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
    double world_hx = 8000/2.0; // this is the max size of current field map
    double world_hy = 8000/2.0;
    double world_hz = 8000/2.0;
    G4Box * solid_world = new G4Box("world",world_hx,world_hy,world_hz);
    f_logic_world = new G4LogicalVolume(solid_world,Air,"world",0,0,0);
    f_physi_world = new G4PVPlacement(0,G4ThreeVector(),f_logic_world,"world", 0,false,0);

    // position of scint1 WRT coil8
    double coil8_to_scint1 = 440; 

    double sci_hx = 380.0/2.0; // dimensions of both scintillators are the same in xy plane
    double sci_hy = 50.0/2.0;
    G4ThreeVector sci1_pos = get_global_pos(coil8_to_scint1);
    G4Box* solid_sci1 = new G4Box("sci1", sci_hx, sci_hy, f_scint1z);
    f_logic_sci1 = new G4LogicalVolume(solid_sci1,Scint,"sci1",0,0,0);
    f_physi_sci1 = new G4PVPlacement(G4Transform3D(rot_36,sci1_pos),f_logic_sci1,"sci1", f_logic_world,false,0);

    //copper target (target) 
    double target_posZ = coil8_to_scint1 + (f_scint1z/2.0) + 3 + (f_targetZ/2.0); // '3' is thickness of separators
    double target_hx = 370.0/2.0;
    double target_hy = 80.0/2.0;
    G4ThreeVector target_pos = get_global_pos(target_posZ);
    G4Box* solid_target = new G4Box("target",target_hx,target_hy,f_targetZ);
    f_logic_target = new G4LogicalVolume(solid_target,f_targetMat,"target",0,0,0);
    f_physi_target = new G4PVPlacement(G4Transform3D(rot_36,target_pos),f_logic_target,"target", f_logic_world,false,0);

   // scintillator bar (sci2)
   // dimensions are the same as scint 1 (other than thickness)
    double scint2_posz = target_posZ + 3 + (f_scint2z/2.0);
    G4ThreeVector sci2_pos = get_global_pos(scint2_posz);
    G4Box* solid_sci2 = new G4Box("sci2", sci_hx, sci_hy, f_scint2z);
    f_logic_sci2 = new G4LogicalVolume(solid_sci2,Scint,"sci2",0,0,0);
    f_physi_sci2 = new G4PVPlacement(G4Transform3D(rot_36,sci2_pos),f_logic_sci2,"sci2", f_logic_world,false,0);

    // degrader
    // degrader is in front of scint 1
    double degrader_posz = coil8_to_scint1 - (f_scint1z/2.0) - 3 - (f_degraderZ/2.0);
    double degrader_hx = 370.0/2.0;
    double degrader_hy = 80.0/2.0;
    G4ThreeVector degrader_pos = get_global_pos(degrader_posz);
    G4Box* solid_degrader = new G4Box("degrader",degrader_hx,degrader_hy,f_degraderZ);
    f_logic_degrader = new G4LogicalVolume(solid_degrader,f_degraderMat,"degrader",0,0,0);
    f_physi_degrader = new G4PVPlacement(G4Transform3D(rot_36,degrader_pos),f_logic_degrader,"degrader", f_logic_world,false,0);



   //-------------------------------------------------------------------------
   // Visual Attributes
   //-------------------------------------------------------------------------
    G4VisAttributes* colR = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
    G4VisAttributes* colG = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
    G4VisAttributes* colB = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
    G4VisAttributes* colLB = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
    f_logic_world->SetVisAttributes(G4VisAttributes::GetInvisible());

    f_logic_sci1->SetVisAttributes(colR);
    f_logic_sci2->SetVisAttributes(colG);
    f_logic_target->SetVisAttributes(colB);
    f_logic_degrader->SetVisAttributes(colLB);
    
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
//    G4LogicalSkinSurface::CleanSurfaceTable();
//    G4LogicalBorderSurface::CleanSurfaceTable();
//    G4SurfaceProperty::CleanSurfacePropertyTable();
    
    //define new one
    G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
    
    f_updated=false;
}