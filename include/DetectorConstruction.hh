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

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class G4Element;
class Field;
class DetectorConstructionMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    ~DetectorConstruction();

public:
    G4VPhysicalVolume* Construct();
    
    // inline changes
    inline void SetSolFieldName(G4String newFile) {f_fname_sol = newFile;};
    inline void SetDipFieldName(G4String newFile) {f_fname_dip = newFile;};

    inline void SetScint1z(double newVal) {f_scint1z = newVal;};
    inline void SetScint2z(double newVal) {f_scint2z = newVal;};
    inline void SetDegraderZ(double newVal) {f_degraderZ = newVal;};
    inline void SetTargetZ(double newVal) {f_targetZ = newVal;};

    // these need some amount of checking
    void SetPolarity(double);
    void SetDegraderMat(G4String);
    void SetTargetMat(G4String);
    
private:
    void DefineMaterials();

    DetectorConstructionMessenger* f_messenger;
    Field* f_myField;// the field map

    G4String f_fname_sol; // solenoid fieldmap
    G4String f_fname_dip; // dipole fieldmap
    double f_dip_polarity; // dipole polarity & scaling
    
    double f_scint1z; // scintillator thickness
    double f_scint2z;

    double f_degraderZ; // degreader thickness
    double f_targetZ; // degreader thickness
    G4Material* f_degraderMat; // degrader material
    G4Material* f_targetMat; // degrader material
    
    G4LogicalVolume*   f_logic_world; // world volume
    G4VPhysicalVolume* f_physi_world;
    
    G4LogicalVolume*   f_logic_sci1; // scint 1 & 2
    G4VPhysicalVolume* f_physi_sci1;
                                   
    G4LogicalVolume*   f_logic_sci2;
    G4VPhysicalVolume* f_physi_sci2;
    
    G4LogicalVolume*   f_logic_target; // target
    G4VPhysicalVolume* f_physi_target;
    
    G4LogicalVolume*   f_logic_degrader; // degrader
    G4VPhysicalVolume* f_physi_degrader;
    
      // pure elements
    G4Material* Pb;
    G4Material* Al;
    G4Material* Mg;
    G4Material* Cu;

      // compounds & mixtures
    G4Material* Air;
    G4Material* Scint;      
    G4Material* Mylar;
    G4Material* Polystyrene;
    G4Material* Polyethylene;
};

#endif

