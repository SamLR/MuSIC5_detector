//
//  PhysicsListMessenger.cc
//  music
//
//  Created by Sam Cook on 28/03/2013.
//
//


#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"

#include "PhysicsListMessenger.hh"
#include "PhysicsList.hh"

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* list)
: physicsList_m(list), setOpticalPhysics_m(NULL)
{
    init();
}

PhysicsListMessenger::~PhysicsListMessenger()
{
    delete setOpticalPhysics_m;
}

void PhysicsListMessenger::init()
{
    // The MuSIC_Detector directory has been created by Detector Construction
    setOpticalPhysics_m =
        new G4UIcmdWithABool("/MuSIC_Detector/enableOpticalProcesses", this);
    setOpticalPhysics_m->SetGuidance("Enable or disable optical processes");
    setOpticalPhysics_m->SetGuidance("Enabling this slows runs massivel");
    setOpticalPhysics_m->SetGuidance("e.g. 15s (disabled) 13m (enabled)");
    setOpticalPhysics_m->SetDefaultValue(false);
}

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newVal)
{
    if (command == setOpticalPhysics_m)
    {
        physicsList_m->EnableOpticalProcesses(
                            setOpticalPhysics_m->GetNewBoolValue(newVal) );
    }
}
