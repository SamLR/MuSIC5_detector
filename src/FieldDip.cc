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

#include "FieldDip.hh"

void FieldDip::global_to_center(double gx, double gy, double gz, double* cx, double* cy, double* cz)
{
   *cy = gy;

   *cx = (gx-coil8_x0)*cos36 - (gz-coil8_z0)*sin36;
   *cz = (gx-coil8_x0)*sin36 + (gz-coil8_z0)*cos36;
   //printf("(gx,gy,gz)=(%lf,%lf,%lf) == (cx,cy,cz)=(%lf,%lf,%lf)\n",gx,gy,gz,*cx,*cy,*cz);
}
int FieldDip::cpos_to_index(double cx, double cy, double cz, int* ix, int* iy, int* iz)
{
   int loc;
   int ix0 = cx /dip_step;
   int iy0 = cy /dip_step;
   int iz0 = cz /dip_step;
   if (abs(ix0) >= MAX_DIP_NX || abs(iy0) >= MAX_DIP_NY || abs(iz0) >= MAX_DIP_NZ) {
      loc = 0;
      return loc;
   }
   if      (ix0>0 && iz0>0 && iy0>0) loc = 1;
   else if (ix0<0 && iz0>0 && iy0>0) loc = 2;
   else if (ix0<0 && iz0<0 && iy0>0) loc = 3;
   else if (ix0>0 && iz0<0 && iy0>0) loc = 4;
   else if (ix0>0 && iz0>0 && iy0<0) loc = -1;
   else if (ix0<0 && iz0>0 && iy0<0) loc = -2;
   else if (ix0<0 && iz0<0 && iy0<0) loc = -3;
   else if (ix0>0 && iz0<0 && iy0<0) loc = -4;


   *ix = abs(ix0);
   *iy = abs(iy0);
   *iz = abs(iz0);

   //printf("ix0 %d iy0 %d iz0 %d --- ix %d iy %d iz %d -- loc %d\n",ix0,iy0,iz0,*ix,*iy,*iz,loc);

   return loc;
}
int FieldDip::gpos_to_index(double x, double y, double z, int* ix, int* iy, int* iz)
{
   double cx,cy,cz;
   global_to_center(x,y,z,&cx,&cy,&cz);
   return cpos_to_index(cx,cy,cz,ix,iy,iz);
}
void FieldDip::set_bfld(double x, double y, double z, double bx, double by, double bz)
{
   int i,j,k;
   cpos_to_index(x,y,z,&i,&j,&k);

   if (i >= MAX_DIP_NX || j >= MAX_DIP_NY || k >= MAX_DIP_NZ) {
      fprintf(stderr,"ERROR: FieldDip::set_bfld overflowed x=%lf y=%lf z=%lf\n",x,y,z);
      exit(1);
   }

   magfld_bx[i][j][k] = bx;
   magfld_by[i][j][k] = by;
   magfld_bz[i][j][k] = bz;
}
FieldDip::FieldDip(G4String fname, int polarity)
{
   /*
      Input file is Dipole BMap XYZ_X0-220_Y0-200-Z0-500.table
      Below is field area in centerline coordinate.
      x=[   0,   220]
      y=[   0,   200]
      z=[   0,   500]
      step is 10 mm
      Polarity is +1 (By>0)
*/

   // Initialize 
   dip_step = 10.0;
   coil8_x0 =  776.3;
   coil8_z0 = 3420.1;
   cos36 = 0.809016994;
   sin36 = 0.587785252; 

   // Read field map
   FILE* fp = fopen(fname.c_str(),"r");
   char line[514];
   if (fp==NULL) {
      fprintf(stderr,"ERROR: failed to open dipole field map %s\n",fname.c_str());
   }
   /* map data is defined in centerline coordinate */
   double in_x;
   double in_y;
   double in_z;
   double in_bx;
   double in_by;
   double in_bz;
   double in_ba;

   while (fgets(line,sizeof(line),fp)) {
      sscanf(line,"%lf %lf %lf %lg %lg %lg %lg",&in_x,&in_y,&in_z,&in_bx,&in_by,&in_bz,&in_ba);
      //printf("(%lf %lf %lf) =  (%lf %lf %lf %lf)\n",line,in_x,in_y,in_z,in_bx,in_by,in_bz,in_ba);

      set_bfld(in_x,in_y,in_z,
            in_bx*polarity,
            in_by*polarity,
            in_bz*polarity);
   }
   fclose(fp);
   printf("Finish to read dipole field map\n");
}

FieldDip::~FieldDip()
{;}
void FieldDip::get_bfield(double x, double y, double z, double *bx, double* by, double *bz)
{
   int ix,iy,iz;
   int loc = gpos_to_index(x,y,z,&ix,&iy,&iz);

   double sx=1.0;
   double sy=1.0;
   double sz=1.0;

   if      (loc==1||loc==-1)  { sx=+1.0; sy=1.0; sz=+1.0; }
   else if (loc==2||loc==-2)  { sx=-1.0; sy=1.0; sz=+1.0; }
   else if (loc==3||loc==-3)  { sx=-1.0; sy=1.0; sz=-1.0; }
   else if (loc==4||loc==-4)  { sx=+1.0; sy=1.0; sz=-1.0; }
   else {
      *bx=0.0;
      *by=0.0;
      *bz=0.0;
      return;
   }

   /*
   printf("(x,y,z)=(%lf,%lf,%lf) (ix,iy,iz)=(%d,%d,%d) (bx,by,bz)=(%lf,%lf,%lf)\n",
         x,y,z,
         ix,iy,iz,
         magfld_bx[ix][iy][iz],
         magfld_by[ix][iy][iz],
         magfld_bz[ix][iy][iz]);
         */

   *bx = sx*magfld_bx[ix][iy][iz];
   *by = sy*magfld_by[ix][iy][iz];
   *bz = sz*magfld_bz[ix][iy][iz];
}

void FieldDip::GetFieldValue(const double Point[3],double *Bfield)
{
   Bfield[0] = 0.0;
   Bfield[1] = 0.0;
   Bfield[2] = 0.0;

   double posx = Point[0];
   double posy = Point[1];
   double posz = Point[2];

   double bx,by,bz;

   //get_bfield(posx,posy,posz,&Bfield[0],&Bfield[1],&Bfield[2]); <= wrong !!! no unit  corrected 2011.9.7
   get_bfield(posx,posy,posz,&bx,&by,&bz);

   Bfield[0] = bx*tesla;
   Bfield[1] = by*tesla;
   Bfield[2] = bz*tesla;


   //fprintf(stderr,"Dipole: %lf %lf %lf = %lf %lf %lf\n",posx,posy,posz,Bfield[0],Bfield[1],Bfield[2]);

}

