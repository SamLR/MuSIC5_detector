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

#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4HEPEvtInterface.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

#include "PrimaryGeneratorActionMessenger.hh"

#include "root.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(Root* root)
:m_root(root), m_g4bl_input_enabled(false), m_g4bl_charged_only(false),
m_x_mean(0.0),    m_y_mean(0.0),       m_z_mean(3901.1799), // z position is pretty much fixed
m_x_sigma(-1.0),  m_y_sigma(-1.0),     m_z_sigma(0.0001),
m_px_mean(0.0),   m_py_mean(0.0),      m_pz_mean(0.0),    m_pz_mean2(0.0),
m_px_sigma(-1.0), m_py_sigma(-1.0),    m_pz_sigma(-1.0),  m_pz_sigma2(-1.0),
m_pz_ratio(0.0),  m_x_offset(-1460.0), m_z_offset(-370.0)
{
    m_messenger = new PrimaryGeneratorActionMessenger(this);
    
    G4int n_particle = 1;
    G4ParticleGun* fParticleGun = new G4ParticleGun(n_particle);
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName;
    G4ParticleDefinition* particle = particleTable->FindParticle(particleName="mu+");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,1.,0.));
    fParticleGun->SetParticleEnergy(100.*GeV);
    fParticleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,0.*cm));
    m_particleGun = fParticleGun;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete m_messenger;
    delete m_particleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // By default assume random, macro controlled position
    // if generated multi particles are required they'll have
    // to be run concurrently with macro
    int pdgid=0;
    G4ThreeVector position(0.0, 0.0, 0.0);
    G4ThreeVector momentum(0.0, 0.0, 0.0);
    
    if (m_g4bl_input_enabled)
    {
        load_from_g4bl(position, momentum, pdgid);
        
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition* particle = particleTable->FindParticle(pdgid);
        m_particleGun->SetParticleDefinition(particle);
        
    } else {
        // PID has already been set
        load_from_dist(position, momentum);
    }
    
    m_particleGun->SetParticleMomentum(momentum);
    m_particleGun->SetParticlePosition(position);
    m_particleGun->GeneratePrimaryVertex(anEvent);

    m_root->g_iev++;
}

void PrimaryGeneratorAction::load_from_g4bl(G4ThreeVector &position,
                                            G4ThreeVector &momentum,
                                            int &pdgid)
{
    m_root->tree_g4bl->GetEntry(m_root->g_iev);
    
    // find only charged particles
    if (m_g4bl_charged_only) {
        while (!(   abs(m_root->in_PDGid) == 11       // electrons or positrons
                 || abs(m_root->in_PDGid) == 13       // muons (+ or -)
                 || abs(m_root->in_PDGid) == 211      // pions (_ or -)
                 || m_root->in_PDGid      == 2212)) { // protons
            m_root->g_iev += 1;
            m_root->tree_g4bl->GetEntry(m_root->g_iev);
        }
    }
    
    pdgid = m_root->in_PDGid;

    // use position and momentum(*_new) in global coordinate
    position.setX(m_root->in_x_new*mm);
    position.setY(m_root->in_y*mm);
    position.setZ(m_root->in_z_new*mm);
    
    momentum.setX(m_root->in_Px_new*MeV);
    momentum.setY(m_root->in_Py*MeV);
    momentum.setZ(m_root->in_Pz_new*MeV);
}

void PrimaryGeneratorAction::load_from_dist(G4ThreeVector &position,
                                            G4ThreeVector &momentum)
{
    // Event & track ID are set to -1 as they are unique to g4bl
    m_root->in_EventID = -1;
    m_root->in_TrackID = -1;
    m_root->in_tof     = 0.0;
    m_root->in_Weight  = 1.0;
    m_root->in_PDGid   = m_particleGun->GetParticleDefinition()->GetPDGEncoding();
    
    // Random position (g4bl co-ordinates)
    double x = G4RandGauss::shoot(m_x_mean, m_x_sigma);
    double y = G4RandGauss::shoot(m_y_mean, m_y_sigma);
    double z = G4RandGauss::shoot(m_z_mean, m_z_sigma);
    // Random momentum (g4bl co-ordinates)
    double Px = G4RandGauss::shoot(m_px_mean, m_px_sigma);
    double Py = G4RandGauss::shoot(m_py_mean, m_py_sigma);
    // Pz given by the sum of two gaussians
    double Pz = shoot_two_gaus(m_pz_mean,  m_pz_sigma,
                               m_pz_mean2, m_pz_sigma2, m_pz_ratio);
    
    // Record the initial random values
    m_root->in_x   = x;
    m_root->in_y   = y;
    m_root->in_z   = z;
    m_root->in_Px  = Px;
    m_root->in_Py  = Py;
    m_root->in_Pz  = Pz;
    
    // Precomputed values for sin and cos of 36degrees
    const double cos_36 = 0.809016994375;
    const double sin_36 = 0.587785252292;
    
    // Offset and rotate by 36 degrees
    x += m_x_offset;
    z += m_z_offset;
    
    const double x_new = x*cos_36 + z*sin_36;
    const double z_new = z*cos_36 - x*sin_36;
    
    // No offset for the momentum, just rotation
    const double Px_new = Px*cos_36 + Pz*sin_36;
    const double Pz_new = Pz*cos_36 - Px*sin_36;
    
    // record the input values
    m_root->in_x_new  = x_new;
    m_root->in_z_new  = z_new;
    m_root->in_Px_new = Px_new;
    m_root->in_Pz_new = Pz_new;
    
    
    // use position and momentum(*_new) in global coordinate
    position = G4ThreeVector(x_new*mm,   y*mm,   z_new*mm);
    momentum = G4ThreeVector(Px_new*MeV, Py*MeV, Pz_new*MeV);
}

double PrimaryGeneratorAction::shoot_two_gaus(const double mean,  const double sigma,
                                              const double mean2, const double sigma2,
                                              const double ratio)
{
    double r = G4UniformRand();
    if (r < ratio)
    {
        return G4RandGauss::shoot(mean, sigma);
    } else
    {
        return G4RandGauss::shoot(mean2, sigma2);
    }
}


