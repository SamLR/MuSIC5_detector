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

#ifndef SteppingAction_H
#define SteppingAction_H 1

#include "globals.hh"
#include "G4UserSteppingAction.hh"
#include "G4String.hh"

#include "Root.hh"

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(Root* root);
    virtual ~SteppingAction() {;};

    virtual void UserSteppingAction(const G4Step*);

  private:
    Root* f_root;

    void truth_hit(const G4Step*);
    void mppc_hit(const G4Step*);
    
    // Helper functions
    static inline int get_process_id (const G4String &process_name);
    static inline int get_volume_id (const G4String &volume_name, const bool entering_degrader);
    static inline bool is_charged (const int pdg_id);
    static inline void issue_max_hit_warning (const G4String &where, const int when);
};

inline int SteppingAction::get_process_id (const G4String &process_name) {
    if      ( process_name == "msc" )                      return 1; // multiple scattering
    else if ( process_name == "Transportation" )           return 2;
    else if ( process_name == "eIoni" )                    return 3;
    else if ( process_name == "eBrem" )                    return 4;
    else if ( process_name == "CoulombScat" )              return 5;
    else if ( process_name == "phot" )                     return 6; // photo electric effect
    else if ( process_name == "compt" )                    return 7;
    else if ( process_name == "muMsc" )                    return 8;
    else if ( process_name == "muIoni" )                   return 9;
    else if ( process_name == "Decay" )                    return 10;
    else if ( process_name == "hIoni" )                    return 11;
    else if ( process_name == "annihil" )                  return 12;
    else if ( process_name == "conv" )                     return 13;
    else if ( process_name == "ionIoni" )                  return 14;
    else if ( process_name == "muMinusCaptureAtRest" )     return 15;
    else if ( process_name == "hadElastic" )               return 16;
    else if ( process_name == "PionPlusInelastic" )        return 17;
    else if ( process_name == "NeutronInelastic" )         return 18;
    else if ( process_name == "CHIPSNuclearCaptureAtRest" )return 19;
    else if ( process_name == "PionMinusInelastic" )       return 20;
    else return  0; // Error!
}

inline int SteppingAction::get_volume_id (const G4String &volume_name,
                                          const bool entering_degrader) {
    if      (entering_degrader           ) return 5;
    else if (volume_name == "u_scint_log") return 1;
    else if (volume_name == "target"     ) return 2;
    else if (volume_name == "d_scint_log") return 3;
    else if (volume_name == "degrader"   ) return 4;
    else if (volume_name == "world"      ) return 0;
    else return -1; // in something else (e.g. MPPC, WLS...)
}

inline bool SteppingAction::is_charged (const int pdg_id) {
    switch (pdg_id) {
        case -211:  return true; // pi-
        case -13:   return true; // mu+
        case -11:   return true; // e+
        case  11:   return true; // e-
        case  13:   return true; // mu-
        case  211:  return true; // pi+
        case  2212: return true; // proton
            
        default: return false;   // ignore everything else
    }
}

inline void SteppingAction::issue_max_hit_warning (const G4String &where, const int when) {
    G4cerr << "SteppingAction Warning: MAX_HIT "
    << MAX_HIT
    << " reached for"
    << where
    << " Event: "
    << when
    << G4endl;
}

#endif

