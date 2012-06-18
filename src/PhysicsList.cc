//
//  PhysicsList.cc
//  music
//
//  Created by Sam Cook on 04/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "PhysicsList.hh"

#include "G4EmStandardPhysics_option3.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh" 
#include "G4EmExtraPhysics.hh" 
#include "G4DecayPhysics.hh" 
#include "G4HadronElasticPhysicsHP.hh" 
#include "G4QStoppingPhysics.hh" 
#include "G4IonPhysics.hh" 
// included to add specific settings
#include "G4MuIonisation.hh"


PhysicsList::PhysicsList() {
    init();
}


void PhysicsList::init(){
    this->defaultCutValue = 50*um;
    this->RegisterPhysics(new G4EmStandardPhysics_option3());    
    this->RegisterPhysics(new HadronPhysicsQGSP_BERT_HP());
    this->RegisterPhysics(new G4EmExtraPhysics());
    this->RegisterPhysics(new G4DecayPhysics());
    this->RegisterPhysics(new G4HadronElasticPhysicsHP());
    this->RegisterPhysics(new G4QStoppingPhysics());
    this->RegisterPhysics(new G4IonPhysics());
}