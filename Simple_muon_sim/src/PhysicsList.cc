//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: PhysicsList.cc,v 1.6 2006-06-29 17:47:21 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//
//  PhysicsList.cc
//  music
//
//  Created by Sam Cook on 04/06/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

//#include <iostream>
#include "PhysicsList.hh"
//
#include "G4EmStandardPhysics_option3.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh"
//#include "G4EmExtraPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4QStoppingPhysics.hh"
#include "G4IonPhysics.hh"


PhysicsList::PhysicsList() {
    init();
}


void PhysicsList::init(){
    this->defaultCutValue = 50*um;
    this->RegisterPhysics(new G4EmStandardPhysics_option3());
    this->RegisterPhysics(new HadronPhysicsQGSP_BERT_HP());
//    this->RegisterPhysics(new G4EmExtraPhysics());
    this->RegisterPhysics(new G4DecayPhysics());
    this->RegisterPhysics(new G4HadronElasticPhysicsHP());
    this->RegisterPhysics(new G4QStoppingPhysics());
    this->RegisterPhysics(new G4IonPhysics());
}

