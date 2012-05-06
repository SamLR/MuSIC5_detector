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
// $Id: exampleN04.cc,v 1.19 2010-06-04 17:50:22 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-04-patch-01 $
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "DetectorConstruction.hh"
#include "QGSP_BERT.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "root.hh"

int main(int argc,char** argv)
{
    if (argc<3) {
        fprintf(stderr,"usage %s <in.root> <out.root> [vis.mac]\n",argv[0]);
        exit(1);
    }
    char* in_root_name = argv[1]; 
    char* out_root_name = argv[2]; 
    char* macro_name = NULL;
    if (argc==4) macro_name=argv[3];

   //root_name = "txt/with_surf_muon/monitor6_By-0T_cor.root";

    printf("argc %d\n",argc);
    printf("in_root_name => \t%s\n",in_root_name);
    printf("out_root_name => \t%s\n",out_root_name);
    if (macro_name) printf("macro_name => \t%s\n",macro_name);

    Root *root = new Root(in_root_name,out_root_name);

    G4RunManager* runManager = new G4RunManager;


   // User Initialization classes (mandatory)
   //
   // variables passed in via macro
    G4VUserDetectorConstruction* detector = new DetectorConstruction();
    runManager->SetUserInitialization(detector);
   //
    G4VUserPhysicsList* physics = new QGSP_BERT();
    runManager->SetUserInitialization(physics);

   // User Action classes
   //
    G4VUserPrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction(root);
    runManager->SetUserAction(gen_action);

   //
   //G4UserRunAction* run_action = new RunAction;
   //runManager->SetUserAction(run_action);
   //
    G4UserEventAction* event_action = new EventAction(root);
    runManager->SetUserAction(event_action);
   //
   //G4UserStackingAction* stacking_action = new StackingAction;
   //runManager->SetUserAction(stacking_action);
   //
   //G4UserTrackingAction* tracking_action = new TrackingAction;
   //runManager->SetUserAction(tracking_action);
   //
    G4UserSteppingAction* stepping_action = new SteppingAction(root);
    runManager->SetUserAction(stepping_action);

//    runManager->Initialize();

#ifdef G4VIS_USE
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
#endif    

   //get the pointer to the User Interface manager   
    G4UImanager* UImanager = G4UImanager::GetUIpointer();  

    if (argc==4) // batch mode
    {
        G4String  command = "/control/execute ";
        G4String filename = macro_name;
        UImanager->ApplyCommand(command+filename);
        runManager->BeamOn(root->nevents_g4bl);
    }
    else           // interactive mode : define UI session
    { 
#ifdef G4UI_USE
        G4UIExecutive * ui = new G4UIExecutive(argc,argv);
#ifdef G4VIS_USE
        UImanager->ApplyCommand("/control/execute vis.mac");     
#endif
        ui->SessionStart();
        delete ui;
#endif

#ifdef G4VIS_USE
        delete visManager;
#endif     
    }


    root->write();


   // Job termination
   // Free the store: user actions, physics_list and detector_description are
   //                 owned and deleted by the run manager, so they should not
   //                 be deleted in the main() program !
    delete runManager;

    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
