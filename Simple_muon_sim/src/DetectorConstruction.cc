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
//
// $Id: DetectorConstruction.cc,v 1.9 2006-06-29 17:47:19 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#include "DetectorConstruction.hh"

//#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"

DetectorConstruction::DetectorConstruction()
    :expHall_log(0), expHall_phys(0), scint1_log(0), scint1_phys(0),
    scint2_log(0), scint2_phys(0)
{;}

DetectorConstruction::~DetectorConstruction() {
    delete scint1_phys;
    delete scint2_phys;
    delete scint1_log;
    delete scint2_log;
    delete expHall_phys;
    delete expHall_log;
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    
    //
    // Use the NIST material manager
    // Not sure if this includes optical properties, need to confirm
    //
    G4NistManager* material_manager = G4NistManager::Instance();
    G4Material* Air = material_manager->FindOrBuildMaterial("G4_AIR");

    //------------------------------------------------------ volumes
    
    //------------------------------ experimental hall (world volume)
    //------------------------------ beam line along x axis
    
    G4double expHall_x = 1.0*m;
    G4double expHall_y = 1.0*m;
    G4double expHall_z = 1.0*m;
    G4Box* expHall_box = new G4Box("expHall_box",expHall_x,expHall_y,expHall_z);
    expHall_log = new G4LogicalVolume(expHall_box, Air, "expHall_log");
    expHall_phys = new G4PVPlacement(0,G4ThreeVector(), expHall_log,"expHall",0,false,0);
    
    G4double scint_x = 10*mm;
    G4double scint_y = 100*mm;
    G4double scint_z = 100*mm;
    G4Box* scint_box = new G4Box("scint_box", scint_x/2, scint_y/2, scint_z/2);
    
    G4double separation = 10*mm;
    G4double x_pos = scint_x/2 + separation/2;

    scint1_log = new G4LogicalVolume(scint_box, Air, "scint1");
    G4ThreeVector scint1_pos(x_pos, 0, 0);
    scint1_phys = new G4PVPlacement(0, scint1_pos, scint1_log, "scint1_p", expHall_log, false, 0);

    scint2_log = new G4LogicalVolume(scint_box, Air, "scint2");
    G4ThreeVector scint2_pos(-x_pos, 0, 0);
    scint2_phys = new G4PVPlacement(0, scint2_pos, scint2_log, "scint2_p", expHall_log, false, 0);

    return expHall_phys;
}

