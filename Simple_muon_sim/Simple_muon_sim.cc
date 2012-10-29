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

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif


int main(int argc,char** argv) {
    
    if (argc<3) {
        G4cout << "usage: " << argv[0] << " <out.root> [vis.mac]"<<G4endl;
        if (argc == 2){
            G4cout << "Received argument: "<< argv[1] << G4endl;
        }
        exit(1);
    }
    G4bool batch_mode = (argc==3);
    G4String out_root_name = G4String(argv[1]);
    G4String macro_name = batch_mode ? G4String(argv[2]) : G4String("");
    
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
    Root* root = new Root(out_root_name);
    EventAction* event = new EventAction(root);
    runManager->SetUserAction(event);
    
    SteppingAction* step = new SteppingAction(root);
    runManager->SetUserAction(step);
    
    // Initialize G4 kernel
    //
    runManager->Initialize();
    
#ifdef G4VIS_USE
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
#endif

    //get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    
    if (batch_mode) {
        G4String command  = "/control/execute ";
        G4String filename = macro_name;
        UImanager->ApplyCommand(command+filename);
    } else {        // interactive mode : define UI session
#ifdef G4UI_USE
        G4UIExecutive * ui = new G4UIExecutive(argc,argv);
    #ifdef G4VIS_USE
        UImanager->ApplyCommand("/control/execute vis.mac");
    #endif
        ui->SessionStart();
        delete ui;
#endif
    }

    // Job termination
    //
    // Free the store: user actions, physics_list and detector_description are
    //                 owned and deleted by the run manager, so they should not
    //                 be deleted in the main() program !
    //
    
#ifdef G4VIS_USE
    delete visManager;
#endif
    delete root;
    delete runManager;
    
    return 0;
}


