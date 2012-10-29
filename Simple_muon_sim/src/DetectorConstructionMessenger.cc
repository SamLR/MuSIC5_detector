//
//  DetectorConstructionMessenger.cc
//  Simple_muon_sim
//
//  Created by Sam Cook on 29/10/2012.
//  Copyright (c) 2012 Sam Cook. All rights reserved.
//



#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"

#include "G4UIdirectory.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* dc)
    : detConstructor_m(dc), updateCmd_m(NULL), UIdirectory_m(NULL),
    targetMaterial_m(NULL), targetThickness_m(NULL) {
    init();
}

DetectorConstructionMessenger::~DetectorConstructionMessenger() {
    delete updateCmd_m;
    delete UIdirectory_m;
    delete targetMaterial_m;
    delete targetThickness_m;
}

void DetectorConstructionMessenger::init()
{
    // set up control the structure
    UIdirectory_m = new G4UIdirectory("/MuSIC_Detector/");
    UIdirectory_m->SetGuidance("Specifies options for the Detector");
    
    // target thickness & material
    targetThickness_m = new G4UIcmdWithADoubleAndUnit("/MuSIC_Detector/targetZ", this);
    targetThickness_m->SetGuidance("Thickness of the target");
    targetThickness_m->SetParameterName("targetZ", true);
    targetThickness_m->SetDefaultValue(10.0);
    targetThickness_m->SetDefaultUnit("mm");
    
    targetMaterial_m = new G4UIcmdWithAString("/MuSIC_Detector/targetMat", this);
    targetMaterial_m->SetGuidance("Set the material to make the target from");
    targetMaterial_m->SetDefaultValue("Cu");
    
    updateCmd_m = new G4UIcommand("/MuSIC_Detector/update",this);
    updateCmd_m->SetGuidance("Update the detector geometry with changed values.");
    updateCmd_m->SetGuidance("Must be run before beamOn if detector has been changed.");
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if ( command == targetThickness_m)
    {
        detConstructor_m->SetTargetX(targetThickness_m->GetNewDoubleValue(newValue));
    }
    else if ( command == targetMaterial_m)
    {
        detConstructor_m->SetTargetMat(newValue);
    }
    else if ( command == updateCmd_m)
    {
        detConstructor_m->UpdateGeometry();
    }
    
}