//
//  PrimaryGeneratorActionMessenger.hh
//  music
//
//  Created by Sam Cook on 20/03/2013.
//
//

#ifndef music_PrimaryGeneratorActionMessenger_hh
#define music_PrimaryGeneratorActionMessenger_hh


#include "G4UImessenger.hh"
#include "globals.hh"

class PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithABool;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorActionMessenger: public G4UImessenger
{
public:
    PrimaryGeneratorActionMessenger(PrimaryGeneratorAction*);
    ~PrimaryGeneratorActionMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
private:
    void init();
    PrimaryGeneratorAction* action;
    G4UIdirectory*          gunDir;
    // Command for setting where to start reading the g4bl file from
    G4UIcmdWithAnInteger*   g4blOffsetCmd;
    // Enable/disable g4bl input
    G4UIcmdWithABool*       g4blEnable;
    
    // When g4bl is off use these to set generator distributions
    G4UIcmdWithADouble* setXmean;
    G4UIcmdWithADouble* setYmean;
    G4UIcmdWithADouble* setZmean;
    G4UIcmdWithADouble* setXsigma;
    G4UIcmdWithADouble* setYsigma;
    G4UIcmdWithADouble* setZsigma;
    G4UIcmdWithADouble* setPxMean;
    G4UIcmdWithADouble* setPyMean;
    G4UIcmdWithADouble* setPzMean;
    G4UIcmdWithADouble* setPzMean2;
    G4UIcmdWithADouble* setPxSigma;
    G4UIcmdWithADouble* setPySigma;
    G4UIcmdWithADouble* setPzSigma;
    G4UIcmdWithADouble* setPzSigma2;
    G4UIcmdWithADouble* setPzRatio;
    G4UIcmdWithADouble* setXoffset;
    G4UIcmdWithADouble* setZoffset;
    
    void create_cmd_with_double(G4UIcmdWithADouble* cmd, const G4String &name,
                                const double default_val = 0.0);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif
