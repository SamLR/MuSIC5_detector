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

#ifndef StackingAction_H
#define StackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"
#include "G4ThreeVector.hh"

class G4Track;

#include "TrackerHit.hh"
#include "MuonHit.hh"
class StackingActionMessenger;

class StackingAction : public G4UserStackingAction
{
  public:
    StackingAction();
    virtual ~StackingAction();

  public:
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage();
    virtual void PrepareNewEvent();

  private:
    G4bool InsideRoI(const G4Track * aTrack,G4double ang);
    G4VHitsCollection* GetCollection(G4String colName);
    
    TrackerHitsCollection* trkHits;
    MuonHitsCollection* muonHits;
    StackingActionMessenger* theMessenger;

    G4int stage;
    G4int reqMuon;
    G4int reqIsoMuon;
    G4int reqIso;
    G4double angRoI;
  
  public:
    inline void SetNRequestMuon(G4int val) { reqMuon = val; }
    inline G4int GetNRequestMuon() const { return reqMuon; }
    inline void SetNRequestIsoMuon(G4int val) { reqIsoMuon = val; }
    inline G4int GetNRequestIsoMuon() const { return reqIsoMuon; }
    inline void SetNIsolation(G4int val) { reqIso = val; }
    inline G4int GetNIsolation() const { return reqIso; }
    inline void SetRoIAngle(G4double val) { angRoI = val; }
    inline G4double GetRoIAngle() const { return angRoI; }
};

#endif

