//
//  DetectorConstructionMessenger.hh
//  Simple_muon_sim
//
//  Created by Sam Cook on 29/10/2012.
//  Copyright (c) 2012 Sam Cook. All rights reserved.
//

#ifndef Simple_muon_sim_DetectorConstructionMessenger_hh
#define Simple_muon_sim_DetectorConstructionMessenger_hh

#include "globals.hh"
#include "G4UImessenger.hh"

// The class we'll be modifying
class DetectorConstruction;
// Creates the UI directory structure
class G4UIdirectory;
// different types of command
class G4UIcommand;
class G4UIcmdWithAString; // for field file names (possible degrader material)
class G4UIcmdWithADouble; // for dipole polarity & scaling
class G4UIcmdWithADoubleAndUnit; // for scint/degrader thicknesses

class DetectorConstructionMessenger : public G4UImessenger
{
public:
    DetectorConstructionMessenger(DetectorConstruction*);
    ~DetectorConstructionMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
private:
    
    void init();
    DetectorConstruction* detConstructor_m;
    
    G4UIcommand*   updateCmd_m;
    G4UIdirectory* UIdirectory_m;
    // material information
    G4UIcmdWithAString* targetMaterial_m; // set the target material
    // component thicknesses
    G4UIcmdWithADoubleAndUnit* targetThickness_m;
};


#endif
