//
//  PhysicsList.hh
//  music
//
//  Created by Sam Cook on 04/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef music_PhysicsList_hh
#define music_PhysicsList_hh

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
//class StepMax;
class PhysicsListMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PhysicsList: public G4VModularPhysicsList {
public:
    PhysicsList();
    ~PhysicsList();
    // In theory should inherit everything we need
    
    void EnableOpticalProcesses(bool newVal);
    
private:
    void init();
    void SetCuts() {;} ;
    
    bool opticalPhysicsEnabled;
    G4VPhysicsConstructor* opticalPhysics;
    PhysicsListMessenger* messenger;
};


#endif
