//
//  PrimaryGeneratorActionMessenger.cc
//  music
//
//  Created by Sam Cook on 20/03/2013.
//
//


#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"


#include "PrimaryGeneratorActionMessenger.hh"
#include "PrimaryGeneratorAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorActionMessenger::PrimaryGeneratorActionMessenger(
                                    PrimaryGeneratorAction* gun)
: action(gun), gunDir(0), offsetCmd(0)
{
    init();
}

PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger()
{
    delete gunDir;
    delete offsetCmd;
}

void PrimaryGeneratorActionMessenger::init()
{
    gunDir = new G4UIdirectory("/MuSIC_Detector/gun/");
    gunDir->SetGuidance("PrimaryGenerator control");
    
    offsetCmd = new G4UIcmdWithAnInteger("/MuSIC_Detector/gun/inputOffset",this);
    offsetCmd->SetGuidance("Offset into input file from which to start read-in.");
    offsetCmd->SetGuidance("  default:0");
    offsetCmd->SetParameterName("Offset", true);
    offsetCmd->SetDefaultValue(0);
    offsetCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command,
                                                  G4String newValue)
{
    if ( command == offsetCmd )
    {
        action->SetEventOffset(offsetCmd->GetNewIntValue(newValue));
    }
}
