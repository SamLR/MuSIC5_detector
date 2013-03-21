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
    G4UIcmdWithAnInteger*   offsetCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif
