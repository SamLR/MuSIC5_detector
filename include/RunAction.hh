//
//  RunAction.hh
//  music
//
//  Created by Sam Cook on 02/04/2013.
//
//

#ifndef music_RunAction_hh
#define music_RunAction_hh

#include "G4UserRunAction.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Run;
class Root;

class RunAction : public G4UserRunAction
{
public:
    RunAction(Root* root);
    ~RunAction() {;} ;
    
public:
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    RunAction();
    
    Root* f_root;
};

#endif
