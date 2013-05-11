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

#include "Root.hh"

SteppingAction::SteppingAction(Root* root) : f_root(root) {;}

void SteppingAction::UserSteppingAction(const G4Step * aStep) {
    // Two types of particle we care about: optical photons
    // and charged particles (11, 13, 211, 2212: e, mu, pi, p).
    // NOTE: optical photons, id = 0 ARE NOT gammas, id = 22
    // 
    // If it's an optical photon log those that hit our MPPCs
    // charged particles we only care about in the main detector
    // components: scintillators, degrader and target, there's also a
    // special counter for particles entering the degrader.
    
    static bool issue_warning_mppc  = true;
    static bool issue_warning_truth = true;
    
    const int pid = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
    
    if ( pid == 0 ) {
        if ((not issue_warning_mppc) and f_root->mppc_hits >= MAX_HIT) {
            return;
            
        } else if (f_root->mppc_hits >= MAX_HIT) {
            issue_warning_mppc = false;
            issue_max_hit_warning(G4String("MPPC"), f_root->g_iev);
            return;
            
        } else {
            issue_warning_mppc = true;
            mppc_hit(aStep);
            return;
        }
        
    } else if (is_charged(pid)) {
        if ((not issue_warning_truth) and f_root->g_nhit >= MAX_HIT) {
            return;
            
        } else if (f_root->g_nhit >= MAX_HIT) {
            issue_warning_truth = false;
            issue_max_hit_warning(G4String("truth"), f_root->g_iev);
            return;
            
        } else {
            issue_warning_truth = true;
            truth_hit(aStep);
            return;
        }
    }
}

void SteppingAction::truth_hit(const G4Step* aStep) {
    
    G4Track* track = aStep->GetTrack();
    
    G4StepPoint* pre_point  = aStep->GetPreStepPoint();
    G4StepPoint* post_point = aStep->GetPostStepPoint();
    
    if (post_point==NULL || pre_point==NULL) return; // Something's gone wrong
    
    // Are we entering or exiting a volume?
    bool last_step  = (post_point->GetStepStatus() == fGeomBoundary);
    bool first_step = (pre_point->GetStepStatus()  == fGeomBoundary);
    
    // ASSUMPTION Record only first and last steps. Need to remove double
    // counts in analysis but this means we should catch particles born inside
    // a volume and exit. We have a problem with partices that exist wholey
    // within a volume but this shouldn't be a problem
    if ( not (first_step || last_step)) return;
    
    // Find out where we are
    const G4VPhysicalVolume* next_phys_vol = post_point->GetTouchableHandle()
                                              ->GetVolume();
    const G4VPhysicalVolume* this_phys_vol = pre_point->GetTouchableHandle()
                                              ->GetVolume();
    
    // No physical volume => entering/exiting world and will segfault when
    // asked for logical volume
    if (!next_phys_vol || !this_phys_vol) return;
    
    const G4LogicalVolume* next_log_vol = next_phys_vol->GetLogicalVolume();
    const G4LogicalVolume* this_log_vol = this_phys_vol->GetLogicalVolume();
    
    const G4String& this_log_vol_name = this_log_vol->GetName();
    const G4String& next_log_vol_name = next_log_vol->GetName();
    
    // Record before we enter the degrader (used as beam profile)
    bool entering_degrader = last_step
                             && (next_log_vol_name == "degrader")
                             && (this_log_vol_name == "world");
    
    int counter_id = get_volume_id(this_log_vol_name, entering_degrader);
    
    if (counter_id < 1) return;
        
    // The array value to write to
    const int g_nhit = f_root->g_nhit;
    // Process name
    const G4String process_name = post_point->GetProcessDefinedStep()->GetProcessName();
    const G4VProcess*  parent_proc = track->GetCreatorProcess();
    const G4String parent_proc_name = (!parent_proc) ? G4String(""): parent_proc->GetProcessName();
    // Parent vertex location name
    const G4String parent_vol_name = track->GetLogicalVolumeAtVertex()->GetName();

    // Record directly into the root class. This is horrible code.
    
    // Information about this step (entering, exiting, where, what etc.)
    f_root->first_step[g_nhit] = first_step;
    f_root->last_step [g_nhit] = last_step;
    f_root->procid    [g_nhit] = get_process_id(process_name);
    f_root->counter   [g_nhit] = counter_id;
    f_root->trkid     [g_nhit] = track->GetTrackID();
    f_root->parentid  [g_nhit] = track->GetParentID();
    f_root->pdgid     [g_nhit] = track->GetDefinition()->GetPDGEncoding();
    // Position
    f_root->x  [g_nhit] = track->GetPosition().x()/mm;
    f_root->y  [g_nhit] = track->GetPosition().y()/mm;
    f_root->z  [g_nhit] = track->GetPosition().z()/mm;
    // Momentum
    f_root->px [g_nhit] = track->GetMomentum().x()/MeV;
    f_root->py [g_nhit] = track->GetMomentum().y()/MeV;
    f_root->pz [g_nhit] = track->GetMomentum().z()/MeV;
    // Parent vertex
    f_root->parent_counter[g_nhit] = get_volume_id(parent_vol_name, false);
    f_root->parent_process[g_nhit] = get_process_id(parent_proc_name);
    // Kinetic energy
    f_root->kinetic [g_nhit] = track->GetKineticEnergy()/MeV;
    // Energy deposited
    f_root->edep [g_nhit]    = aStep->GetTotalEnergyDeposit()/MeV;
    // How long (in simulated time) since the start of the event?
    f_root->tof [g_nhit]     = track->GetGlobalTime()/ns;
    // Increment the recorder pointer
    f_root->g_nhit++;
}

void SteppingAction::mppc_hit(const G4Step* aStep) {
    
    // Check if this is the first step in a volume?
    const G4StepPoint* pre_point = aStep->GetPreStepPoint();
    const bool first_step        = (pre_point->GetStepStatus() == fGeomBoundary);
    
    if (not first_step) return;
    
    // Now check that we're in an MPPC
    const G4VPhysicalVolume* this_vol = pre_point->GetTouchableHandle()->GetVolume();
    const G4String log_vol_name       = this_vol->GetLogicalVolume()->GetName();
    
    if (log_vol_name != "mppc_log") return;
    
    // Log the info
    G4Track * track = aStep->GetTrack();
    
    const int hit_n = f_root->mppc_hits;
    
    f_root->mppc_x    [hit_n] = track->GetPosition().x()/mm;
    f_root->mppc_y    [hit_n] = track->GetPosition().y()/mm;
    f_root->mppc_z    [hit_n] = track->GetPosition().z()/mm;
    f_root->mppc_time [hit_n] = track->GetGlobalTime()/ns;
    f_root->mppc_hits++;
}






