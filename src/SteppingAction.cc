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

void SteppingAction::set_hit(int acounter, const char* aprocname, int atrkid, int aparentid, int apdgid, double ax, double ay, double az, double apx, double apy, double apz, double akinetic, double aedep, double atof)
{
    if (f_root->g_nhit>=MAX_HIT) {
        return;
    }

    int g_nhit = f_root->g_nhit;
    if (strcmp(aprocname,"msc")==0)                            {  f_root->procid[g_nhit]=1; }
    else if (strcmp(aprocname,"Transportation")==0)            {  f_root->procid[g_nhit]=2; }
    else if (strcmp(aprocname,"eIoni")==0)                     {  f_root->procid[g_nhit]=3; }
    else if (strcmp(aprocname,"eBrem")==0)                     {  f_root->procid[g_nhit]=4; }
    else if (strcmp(aprocname,"CoulombScat")==0)               {  f_root->procid[g_nhit]=5; }
    else if (strcmp(aprocname,"phot")==0)                      {  f_root->procid[g_nhit]=6; }
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
    int parentid = aStep->GetTrack()->GetParentID();
    int trkid = aStep->GetTrack()->GetTrackID();
    int pdgid  = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
    double kinetic = aStep->GetTrack()->GetKineticEnergy()/MeV;
    double x = aStep->GetPreStepPoint()->GetPosition().x()/cm;
    double y = aStep->GetPreStepPoint()->GetPosition().y()/cm;
    double z = aStep->GetPreStepPoint()->GetPosition().z()/cm;
    double px = aStep->GetPreStepPoint()->GetMomentum().x()/MeV;
    double py = aStep->GetPreStepPoint()->GetMomentum().y()/MeV;
    double pz = aStep->GetPreStepPoint()->GetMomentum().z()/MeV;
    double edep = aStep->GetTotalEnergyDeposit()/MeV;
    double tof = aStep->GetTrack()->GetGlobalTime()/ns;
    const G4String& volname = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName();
    int acounter=1;
    if      (strcmp(volname,"sci1")==0)     { acounter = 1; } 
    else if (strcmp(volname,"target")==0)   { acounter = 2; } 
    else if (strcmp(volname,"sci2")==0)     { acounter = 3; }
    else return;

    const G4VProcess * vp = aStep->GetPreStepPoint()->GetProcessDefinedStep();
    if (vp==NULL) {
       return;
    }
    const G4String& procname = vp->GetProcessName();

    set_hit(acounter,procname.c_str(),trkid,parentid,pdgid,x,y,z,px,py,pz,kinetic,edep,tof);
}


