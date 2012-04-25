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

#ifndef FieldSol_H
#define FieldSol_H 1

#include "globals.hh"

class FieldSol
{
   public:
      FieldSol(G4String fname);
      ~FieldSol();

      void GetFieldValue( const  double Point[3], double *Bfield );

   private:

#define MAX_SOL_NX 25
#define MAX_SOL_NY 13
#define MAX_SOL_NZ 25
      double magfld_bx[MAX_SOL_NX][MAX_SOL_NY][MAX_SOL_NZ];
      double magfld_by[MAX_SOL_NX][MAX_SOL_NY][MAX_SOL_NZ];
      double magfld_bz[MAX_SOL_NX][MAX_SOL_NY][MAX_SOL_NZ];

      void index_to_pos(int ix, int iy, int iz, double *x, double *y, double *z);
      void pos_to_index(double x, double y, double z, int* ix, int* iy, int* iz);
      void set_bfld(double x, double y, double z, double bx, double by, double bz);
      void get_bfld(double x, double y, double z, double *bx, double *by, double *bz);
      void get_bfield(double x, double y, double z, double *bx, double* by, double *bz);

      double sol_step; // step size in mm
      double region_xmin; // solenoid field map region in global coordinate [mm]
      double region_xmax;
      double region_ymin;
      double region_ymax;
      double region_zmin;
      double region_zmax;
};

#endif

