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

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4ParticleGun.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include "Root.hh"

class G4VPrimaryGenerator;
class G4Event;
class PrimaryGeneratorActionMessenger;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(Root* root);
    ~PrimaryGeneratorAction();

    inline void set_event_offset (const int offset) { m_root->g_iev = offset; };
    void GeneratePrimaries(G4Event* anEvent);
    G4ParticleGun* m_particleGun;
    
    // Enable/disable g4bl
    inline void set_g4bl_mode(const bool new_val) {
        m_g4bl_input_enabled = new_val;
        m_root->g4bl_file_enabled = new_val;
    };
    inline void set_charged (const bool new_val) {m_g4bl_charged_only = new_val;} ;

    // Setters for all of the paramters
    // We want these so we can use macros to set profiles for different
    // particle types
    inline void set_x_mean    (const double new_val) {m_x_mean    = new_val; };
    inline void set_y_mean    (const double new_val) {m_y_mean    = new_val; };
    inline void set_z_mean    (const double new_val) {m_z_mean    = new_val; };
    inline void set_x_sigma   (const double new_val) {m_x_sigma   = new_val; };
    inline void set_y_sigma   (const double new_val) {m_y_sigma   = new_val; };
    inline void set_z_sigma   (const double new_val) {m_z_sigma   = new_val; };
    inline void set_px_mean   (const double new_val) {m_px_mean   = new_val; };
    inline void set_py_mean   (const double new_val) {m_py_mean   = new_val; };
    inline void set_pz_mean   (const double new_val) {m_pz_mean   = new_val; };
    inline void set_pz_mean2  (const double new_val) {m_pz_mean2  = new_val; };
    inline void set_px_sigma  (const double new_val) {m_px_sigma  = new_val; };
    inline void set_py_sigma  (const double new_val) {m_py_sigma  = new_val; };
    inline void set_pz_sigma  (const double new_val) {m_pz_sigma  = new_val; };
    inline void set_pz_sigma2 (const double new_val) {m_pz_sigma2 = new_val; };
    inline void set_pz_ratio  (const double new_val) {m_pz_ratio  = new_val; };
    inline void set_x_offset  (const double new_val) {m_x_offset  = new_val; };
    inline void set_z_offset  (const double new_val) {m_z_offset  = new_val; };
    
  private:
    Root* m_root;
    PrimaryGeneratorActionMessenger* m_messenger;
    
    // The z-momentum is modelled as the sum of two gaussians
    double static shoot_two_gaus(const double mean,  const double sigma,
                                 const double mean2, const double sigma2,
                                 const double ratio);
    
    void load_from_g4bl(G4ThreeVector &position, G4ThreeVector &momentum, int &pdgid);
    void load_from_dist(G4ThreeVector &position, G4ThreeVector &momentum);
    
    bool m_g4bl_input_enabled;
    bool m_g4bl_charged_only;
    // Variables for creating gaussian distributions, set via messenger
    double m_x_mean,   m_y_mean,   m_z_mean;
    double m_x_sigma,  m_y_sigma,  m_z_sigma;
    // Pz is fitted with two gaussians rather than 1
    double m_px_mean,  m_py_mean,  m_pz_mean,  m_pz_mean2;
    double m_px_sigma, m_py_sigma, m_pz_sigma, m_pz_sigma2;
    // The ratio of the integral of the first to the second gaussian
    double m_pz_ratio;
    // The offsets that the x & z positions have for proper transformation
    double m_x_offset, m_z_offset;
    
};

#endif


