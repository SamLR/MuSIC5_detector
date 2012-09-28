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

#include "SteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

#include "root.hh"

SteppingAction::SteppingAction(Root* root)
{ f_root = root;}

SteppingAction::~SteppingAction()
{;}

void SteppingAction::set_hit(bool first_step, bool last_step, int acounter, const char* aprocname, int atrkid, int aparentid, int apdgid, double ax, double ay, double az, double apx, double apy, double apz, double akinetic, double aedep, double atof)
{
    if (f_root->g_nhit>=MAX_HIT) {
        return;
    }
    
    int g_nhit = f_root->g_nhit;
    if (strcmp(aprocname,"msc")==0)                            {  f_root->procid[g_nhit]=1; } // multiple scattering
    else if (strcmp(aprocname,"Transportation")==0)            {  f_root->procid[g_nhit]=2; }
    else if (strcmp(aprocname,"eIoni")==0)                     {  f_root->procid[g_nhit]=3; }
    else if (strcmp(aprocname,"eBrem")==0)                     {  f_root->procid[g_nhit]=4; }
    else if (strcmp(aprocname,"CoulombScat")==0)               {  f_root->procid[g_nhit]=5; }
    else if (strcmp(aprocname,"phot")==0)                      {  f_root->procid[g_nhit]=6; } // photo electric effect
    else if (strcmp(aprocname,"compt")==0)                     {  f_root->procid[g_nhit]=7; }
    else if (strcmp(aprocname,"muMsc")==0)                     {  f_root->procid[g_nhit]=8; }
    else if (strcmp(aprocname,"muIoni")==0)                    {  f_root->procid[g_nhit]=9; }
    else if (strcmp(aprocname,"Decay")==0)                     {  f_root->procid[g_nhit]=10; }
    else if (strcmp(aprocname,"hIoni")==0)                     {  f_root->procid[g_nhit]=11; }
    else if (strcmp(aprocname,"annihil")==0)                   {  f_root->procid[g_nhit]=12; }
    else if (strcmp(aprocname,"conv")==0)                      {  f_root->procid[g_nhit]=13; }
    else if (strcmp(aprocname,"ionIoni")==0)                   {  f_root->procid[g_nhit]=14; }
    else if (strcmp(aprocname,"muMinusCaptureAtRest")==0)      {  f_root->procid[g_nhit]=15; }
    else if (strcmp(aprocname,"hadElastic")==0)                {  f_root->procid[g_nhit]=16; }
    else if (strcmp(aprocname,"PionPlusInelastic")==0)         {  f_root->procid[g_nhit]=17; }
    else if (strcmp(aprocname,"NeutronInelastic")==0)          {  f_root->procid[g_nhit]=18; }
    else if (strcmp(aprocname,"CHIPSNuclearCaptureAtRest")==0) {  f_root->procid[g_nhit]=19; }
    else if (strcmp(aprocname,"PionMinusInelastic")==0)        {  f_root->procid[g_nhit]=20; }
    
    f_root->first_step[g_nhit] = first_step;
    f_root->last_step[g_nhit] = last_step;
    f_root->counter[g_nhit] = acounter;
    f_root->trkid[g_nhit] = atrkid;
    f_root->parentid[g_nhit] = aparentid;
    f_root->pdgid[g_nhit] = apdgid;
    f_root->x[g_nhit] = ax;
    f_root->y[g_nhit] = ay;
    f_root->z[g_nhit] = az;
    f_root->px[g_nhit] = apx;
    f_root->py[g_nhit] = apy;
    f_root->pz[g_nhit] = apz;
    f_root->kinetic[g_nhit] = akinetic;
    f_root->edep[g_nhit] = aedep;
    f_root->tof[g_nhit] = atof;
    f_root->g_nhit++;
}

void SteppingAction::UserSteppingAction(const G4Step * aStep)
{ 
    G4Track * track = aStep->GetTrack();
    G4StepPoint* point1 = aStep->GetPreStepPoint();
    G4StepPoint* point2 = aStep->GetPostStepPoint();
    
    if (point2==NULL || point1==NULL) return;
    
    const G4VPhysicalVolume* nextvol = point2->GetTouchableHandle()->GetVolume();
    
    if (!nextvol) return; // if there is no next volume we're exiting the world
    
    bool last_step  = (point2->GetStepStatus() == fGeomBoundary);
    bool first_step = (point1->GetStepStatus() == fGeomBoundary);
    const G4String& volname = point1->GetTouchableHandle()->GetVolume()->GetName();
    
    bool entering_scint1 = last_step &&
        (point2->GetTouchableHandle()->GetVolume()->GetName() == "degrader") &&
        (volname == "world");
    
    int acounter = 0; // main step
    if     ( volname == "sci1" )     { acounter = 1; } 
    else if( volname == "target" )   { acounter = 2; } 
    else if( volname == "sci2" )     { acounter = 3; }
    else if( volname == "degrader" ) { acounter = 4; }
    else if( entering_scint1 )       { acounter = 5; }
    else return; // not in a volume
    // record stuff specially if we're about to enter the degrader or st
    
    int parentid = track->GetParentID();
    int trkid = track->GetTrackID();
    int pdgid  = track->GetDefinition()->GetPDGEncoding();
    double kinetic = track->GetKineticEnergy()/MeV;
    double tof = track->GetGlobalTime()/ns;
    double x = track->GetPosition().x()/mm;
    double y = track->GetPosition().y()/mm;
    double z = track->GetPosition().z()/mm;
    double px = track->GetMomentum().x()/MeV;
    double py = track->GetMomentum().y()/MeV;
    double pz = track->GetMomentum().z()/MeV;
    double edep = aStep->GetTotalEnergyDeposit()/MeV;
    const G4String& procname = point2->GetProcessDefinedStep()->GetProcessName();
    
    set_hit(first_step, last_step, acounter,procname.c_str(),trkid,parentid,pdgid,x,y,z,px,py,pz,kinetic,edep,tof);
}

