//
//  PhysicsList.cc
//  music
//
//  Created by Sam Cook on 04/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "G4EmStandardPhysics_option3.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh" 
#include "G4EmExtraPhysics.hh" 
#include "G4DecayPhysics.hh" 
#include "G4HadronElasticPhysicsHP.hh" 
#include "G4QStoppingPhysics.hh" 
#include "G4IonPhysics.hh" 
#include "G4OpticalPhysics.hh"
// included to add specific settings
#include "G4MuIonisation.hh"

#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"


PhysicsList::PhysicsList()
: opticalPhysicsEnabled(false), opticalPhysics(NULL), messenger(NULL)
{
    messenger = new PhysicsListMessenger(this);
    init();
    if (opticalPhysicsEnabled) EnableOpticalProcesses(opticalPhysicsEnabled);
}

PhysicsList::~PhysicsList()
{ 
    delete messenger;
}


void PhysicsList::init()
{ // Set a basic physics set, that doesn't include optical processes
    this->defaultCutValue = 50*um;
    this->RegisterPhysics(new G4EmStandardPhysics_option3());    
    this->RegisterPhysics(new HadronPhysicsQGSP_BERT_HP());
    this->RegisterPhysics(new G4EmExtraPhysics());
    this->RegisterPhysics(new G4DecayPhysics());
    this->RegisterPhysics(new G4HadronElasticPhysicsHP());
    this->RegisterPhysics(new G4QStoppingPhysics());
    this->RegisterPhysics(new G4IonPhysics());
}

void PhysicsList::EnableOpticalProcesses(bool newVal)
{
    opticalPhysicsEnabled=newVal;
    
    if (opticalPhysicsEnabled)
    { // enable optical processes
        // if there are already optical processes enabled: tidy them up
        if (opticalPhysics) this->RemovePhysics(opticalPhysics);
        opticalPhysics = new G4OpticalPhysics();
        this->RegisterPhysics(opticalPhysics);
    } else if (opticalPhysics)
    { // If optical processes had been enabled tidy them up
        this->RemovePhysics(opticalPhysics);
    }
}
