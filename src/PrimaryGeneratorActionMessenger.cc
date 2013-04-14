//
//  PrimaryGeneratorActionMessenger.cc
//  music
//
//  Created by Sam Cook on 20/03/2013.
//
//


#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"

#include "PrimaryGeneratorActionMessenger.hh"
#include "PrimaryGeneratorAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorActionMessenger::PrimaryGeneratorActionMessenger(
                                    PrimaryGeneratorAction* gun)
: action(gun), gunDir(0), g4blOffsetCmd(0)
{
    init();
}

PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger()
{
    delete gunDir;
    delete g4blOffsetCmd;
}

void PrimaryGeneratorActionMessenger::init()
{
    // Create a directory for these commands
    gunDir = new G4UIdirectory("/MuSIC_Detector/gun/");
    gunDir->SetGuidance("PrimaryGenerator control");
    
    // Command for adjusting the offset into the g4bl file
    g4blOffsetCmd = new G4UIcmdWithAnInteger("/MuSIC_Detector/gun/inputOffset",this);
    g4blOffsetCmd->SetGuidance("Offset into input file from which to start read-in.");
    g4blOffsetCmd->SetGuidance("  default:0");
    g4blOffsetCmd->SetParameterName("Offset", true);
    g4blOffsetCmd->SetDefaultValue(0);
    g4blOffsetCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    // Enable use of the g4bl particles
    g4blEnable = new G4UIcmdWithABool("/MuSIC_Detector/gun/g4blEnable", this);
    g4blEnable->SetGuidance("Enable the g4bl file as particle input");
    g4blEnable->SetParameterName("Enable", true);
    g4blEnable->SetDefaultValue(false);
    g4blOffsetCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    // Toggle using only charged particles from G4BL
    g4blCharged = new G4UIcmdWithABool("/MuSIC_Detector/gun/g4blCharged", this);
    g4blCharged->SetGuidance("Set input particles to only be charged");
    g4blCharged->SetParameterName("Enabled", true);
    g4blCharged->SetDefaultValue(false);
    g4blCharged->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    // Create the cmds for setting the gaussian value
    // Position means
    setXmean = create_cmd_with_double("setXmean");
    setYmean = create_cmd_with_double("setYmean");
    setZmean = create_cmd_with_double("setZmean", 3901.1799);
    // Position sigmas
    setXsigma = create_cmd_with_double("setXsigma");
    setYsigma = create_cmd_with_double("setYsigma");
    setZsigma = create_cmd_with_double("setZsigma", 0.001);
    // Momentum means
    setPxMean  = create_cmd_with_double("setPxMean");
    setPyMean  = create_cmd_with_double("setPyMean");
    setPzMean  = create_cmd_with_double("setPzMean");
    setPzMean2 = create_cmd_with_double("setPzMean2");
    // Momentum Sigmas
    setPxSigma  = create_cmd_with_double("setPxSigma");
    setPySigma  = create_cmd_with_double("setPySigma");
    setPzSigma  = create_cmd_with_double("setPzSigma");
    setPzSigma2 = create_cmd_with_double("setPzSigma2");
    
    // Odd commands
    // Set the ratio between gaus1(Pz) and gaus2(Pz)
    setPzRatio = create_cmd_with_double("setPzRatio");
    // These probably shouldn't be used...
    // Set the positional offsets for the rotation
    setXoffset = create_cmd_with_double("setXoffset",-1460.0);
    setZoffset = create_cmd_with_double("setZoffset", -370.0);
}

void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command,
                                                  G4String newValue)
{
    if ( command == g4blOffsetCmd ){
        action->set_event_offset(g4blOffsetCmd->GetNewIntValue(newValue));
    }
    else if ( command == g4blEnable ){
        action->set_g4bl_mode(g4blEnable->GetNewBoolValue(newValue));
    }
    else if ( command == g4blCharged ) {
        action->set_charged(g4blCharged->GetNewBoolValue(newValue));
    }
    else if ( command == setXmean    ) {
        action->set_x_mean(setXmean->GetNewDoubleValue(newValue));
    }
    else if ( command == setYmean    ) {
        action->set_y_mean(setYmean->GetNewDoubleValue(newValue));
    }
    else if ( command == setZmean    ) {
        action->set_z_mean(setZmean->GetNewDoubleValue(newValue));
    }
    else if ( command == setXsigma   ) {
        action->set_x_sigma(setXsigma->GetNewDoubleValue(newValue));
    }
    else if ( command == setYsigma   ) {
        action->set_y_sigma(setYsigma->GetNewDoubleValue(newValue));
    }
    else if ( command == setZsigma   ) {
        action->set_z_sigma(setZsigma->GetNewDoubleValue(newValue));
    }
    else if ( command == setPxMean   ) {
        action->set_px_mean(setPxMean->GetNewDoubleValue(newValue));
    }
    else if ( command == setPyMean   ) {
        action->set_py_mean(setPyMean->GetNewDoubleValue(newValue));
    }
    else if ( command == setPzMean   ) {
        action->set_pz_mean(setPzMean->GetNewDoubleValue(newValue));
    }
    else if ( command == setPzMean2  ) {
        action->set_pz_mean2(setPzMean2->GetNewDoubleValue(newValue));
    }
    else if ( command == setPxSigma  ) {
        action->set_px_sigma(setPxSigma->GetNewDoubleValue(newValue));
    }
    else if ( command == setPySigma  ) {
        action->set_py_sigma(setPySigma->GetNewDoubleValue(newValue));
    }
    else if ( command == setPzSigma  ) {
        action->set_pz_sigma(setPzSigma->GetNewDoubleValue(newValue));
    }
    else if ( command == setPzSigma2 ) {
        action->set_pz_sigma2(setPzSigma2->GetNewDoubleValue(newValue));
    } 
    else if ( command == setPzRatio  ) {
        action->set_pz_ratio(setPzRatio->GetNewDoubleValue(newValue));
    }
    else if ( command == setXoffset ) {
        action->set_x_offset(setXoffset->GetNewDoubleValue(newValue));
    }
    else if ( command == setZoffset ) {
        action->set_z_offset(setZoffset->GetNewDoubleValue(newValue));
    }
}

G4UIcmdWithADouble* PrimaryGeneratorActionMessenger::create_cmd_with_double(
                                    const G4String &name,
                                    const double default_val)
{
    // Ton of setters for the distribution generators
    G4String path = G4String("/MuSIC_Detector/gun/") + name;
    G4UIcmdWithADouble* cmd = new G4UIcmdWithADouble(path, this);
    cmd->SetGuidance(name);
    cmd->SetParameterName(name, true);
    cmd->SetDefaultValue(default_val);
    cmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    return cmd;
}
