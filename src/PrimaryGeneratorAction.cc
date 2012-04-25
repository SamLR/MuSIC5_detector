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

#include "root.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(Root* root)
{
   f_root = root;

   G4int n_particle = 1;
   G4ParticleGun* fParticleGun = new G4ParticleGun(n_particle);
   G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
   G4String particleName;
   G4ParticleDefinition* particle = particleTable->FindParticle(particleName="mu+");
   fParticleGun->SetParticleDefinition(particle);
   fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,1.,0.));
   fParticleGun->SetParticleEnergy(100.*GeV);
   fParticleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,0.*cm));
   particleGun = fParticleGun;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
   delete particleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
   f_root->tree_g4bl->GetEntry(f_root->g_iev);
   int pdgid = f_root->in_PDGid;
   // use position and momentum(*_new) in global coordinate
   double posx = f_root->in_x_new*mm;
   double posy = f_root->in_y*mm;
   double posz = f_root->in_z_new*mm;
   double momx = f_root->in_Px_new*MeV;
   double momy = f_root->in_Py*MeV;
   double momz = f_root->in_Pz_new*MeV;
   //printf("iev %d pdgid %d\n",g_iev, pdgid);
   //printf("iev %d posx %lf posy %lf posz %lf\n", g_iev,posx/mm,posy/mm,posz/mm);
   //printf("iev %d momx %lf momy %lf momz %lf\n", g_iev,momx/MeV,momy/MeV,momz/MeV);

   G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
   G4ParticleDefinition* particle = particleTable->FindParticle(pdgid);
   //G4ParticleDefinition* particle = particleTable->FindParticle("chargedgeantino");
   particleGun->SetParticleDefinition(particle);

   particleGun->SetParticleMomentum(G4ThreeVector(momx,momy,momz));
   particleGun->SetParticlePosition(G4ThreeVector(posx,posy,posz));
   //printf("posx %lf posy %lf posz %lf\n",posx,posy,posz);

   particleGun->GeneratePrimaryVertex(anEvent);

   f_root->g_iev++;
}


