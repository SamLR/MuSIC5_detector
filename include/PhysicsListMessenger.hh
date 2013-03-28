//
//  PhysicsListMessenger.h
//  music
//
//  Created by Sam Cook on 28/03/2013.
//
//

#ifndef __music__PhysicsListMessenger__
#define __music__PhysicsListMessenger__ 1

#include <iostream>
#include "globals.hh"
#include "G4UImessenger.hh"

class PhysicsList;
class G4UIcommand;
class G4UIcmdWithABool;

class PhysicsListMessenger : public G4UImessenger
{
public:
    PhysicsListMessenger(PhysicsList*);
    ~PhysicsListMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
private:
    PhysicsListMessenger();
    PhysicsListMessenger(PhysicsListMessenger &);

    void init();
    PhysicsList* physicsList_m;
    G4UIcmdWithABool* setOpticalPhysics_m;
};

#endif /* defined(__music__PhysicsListMessenger__) */
