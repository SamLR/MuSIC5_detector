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
// $Id: exampleN01.cc,v 1.6 2006-06-29 17:47:10 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// --------------------------------------------------------------
//      GEANT 4 - exampleN01
// --------------------------------------------------------------

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "TString.h"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "Root.hh"
#include "SteppingAction.hh"
#include "EventAction.hh"

int main()
{
    // Construct the default run manager
    //
    G4RunManager* runManager = new G4RunManager;
    
    // set mandatory initialization classes
    //
    G4VUserDetectorConstruction* detector = new DetectorConstruction;
    runManager->SetUserInitialization(detector);
    //
    PhysicsList* physics = new PhysicsList;
    
    runManager->SetUserInitialization(physics);
    
    // set mandatory user action class
    //
    PrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction;
    runManager->SetUserAction(gen_action);
    
    
    // Make the root object and set the stepping & event actions
    TString root_file_name = TString("out.root");
    Root* root = new Root(root_file_name);
    EventAction* event = new EventAction(root);
    runManager->SetUserAction(event);
    
    SteppingAction* step = new SteppingAction(root);
    runManager->SetUserAction(step);
    
    // Initialize G4 kernel
    //
    runManager->Initialize();
    
    // Get the pointer to the UI manager and set verbosities
    //
    G4UImanager* UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/run/verbose 1");
    UI->ApplyCommand("/event/verbose 1");
    UI->ApplyCommand("/tracking/verbose 1");
    
    // Start a run
    //
    G4int numberOfEvent = 3;
    runManager->BeamOn(numberOfEvent);
    
    // Job termination
    //
    // Free the store: user actions, physics_list and detector_description are
    //                 owned and deleted by the run manager, so they should not
    //                 be deleted in the main() program !
    //
    delete root;
    delete runManager;
    
    return 0;
}


