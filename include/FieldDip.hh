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

#ifndef FieldDip_H
#define FieldDip_H 1

#include "globals.hh"

class FieldDip
{
   public:
      FieldDip(G4String fname, int polarity);
      ~FieldDip();

      void GetFieldValue( const  double Point[3], double *Bfield );

   private:

#define MAX_DIP_NX 23
#define MAX_DIP_NY 21
#define MAX_DIP_NZ 51
      double magfld_bx[MAX_DIP_NX][MAX_DIP_NY][MAX_DIP_NZ];
      double magfld_by[MAX_DIP_NX][MAX_DIP_NY][MAX_DIP_NZ];
      double magfld_bz[MAX_DIP_NX][MAX_DIP_NY][MAX_DIP_NZ];

      int gpos_to_index(double x, double y, double z, int* ix, int* iy, int* iz);
      int cpos_to_index(double x, double y, double z, int* ix, int* iy, int* iz);
      void set_bfld(double x, double y, double z, double bx, double by, double bz);
      void get_bfield(double x, double y, double z, double *bx, double* by, double *bz);

      void global_to_center(double gx, double gy, double gz, double* cx, double* cy, double* cz);


      double coil8_x0; //coil8 (last coil of dipole magnet) in global coordinate [mm]
      double coil8_z0;
      double cos36;
      double sin36;

      double dip_step; // step size in mm
};

#endif

