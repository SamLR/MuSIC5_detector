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
// $Id: DetectorConstruction.hh,v 1.6 2006-06-29 17:47:13 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//

#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class DetectorConstructionMessenger;

#include "G4VUserDetectorConstruction.hh"
#include "G4String.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction();

    G4VPhysicalVolume* Construct();
    
    void SetTargetX(G4double thickness);
    void SetTargetMat(G4String new_material);
    void UpdateGeometry();

  private:
    // macro-able settings
    //
    G4double separation;
    G4double st_x;
    G4Material* st_mat;
    G4String mat_name;
    DetectorConstructionMessenger* messenger;
    
    // Logical volumes
    //
    G4LogicalVolume* expHall_log;
    G4LogicalVolume* scint1_log;
    G4LogicalVolume* scint2_log;
    G4LogicalVolume* st_log;
    
    // Physical volumes
    //
    G4VPhysicalVolume* expHall_phys;
    G4VPhysicalVolume* scint1_phys;
    G4VPhysicalVolume* scint2_phys;
    G4VPhysicalVolume* st_phys;
};

#endif

