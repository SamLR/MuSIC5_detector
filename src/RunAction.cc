//
//  RunAction.cc
//  music
//
//  Created by Sam Cook on 02/04/2013.
//
//

#include <stdio.h>
#include "RunAction.hh"
#include "Root.hh"

RunAction::RunAction(Root* root): f_root(root){;}

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
    f_root->init_root();
}

void RunAction::EndOfRunAction(const G4Run* aRun)
{
    
}