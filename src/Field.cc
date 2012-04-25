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

#include "Field.hh"

Field::Field(G4String fname_sol, G4String fname_dip, int polarity)
{
   field_sol = new FieldSol(fname_sol);
   field_dip = new FieldDip(fname_dip,polarity);
}

Field::~Field()
{
   delete field_dip;
   delete field_sol;
}
void Field::GetFieldValue(const double Point[3],double *Bfield) const
{
    double bfield_sol[3];
    double bfield_dip[3];
    
    field_sol->GetFieldValue(Point,bfield_sol);
    field_dip->GetFieldValue(Point,bfield_dip);

    Bfield[0] = bfield_sol[0] + bfield_dip[0];
    Bfield[1] = bfield_sol[1] + bfield_dip[1];
    Bfield[2] = bfield_sol[2] + bfield_dip[2];

    //printf("%lf %lf %lf = %lf %lf %lf\n",posx,posy,posz,Bfield[0],Bfield[1],Bfield[2]);
}

