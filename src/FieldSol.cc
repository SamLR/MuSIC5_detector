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

#include "FieldSol.hh"

void FieldSol::index_to_pos(int ix, int iy, int iz, double *x, double *y, double *z)
{
    *x = region_xmin + ix*sol_step;
    *y = 0.0         + iy*sol_step;
    *z = region_xmin + iz*sol_step;
    
}
void FieldSol::pos_to_index(double x, double y, double z, int* ix, int* iy, int* iz)
{
//    *ix = (int) (x - region_xmin)/sol_step;
//    *iy = (int) (y - 0.0        )/sol_step;
//    *iz = (int) (z - region_zmin)/
    *ix = abs (static_cast<int>((x - region_xmin)/sol_step));
    *iy = abs (static_cast<int>((y - 0.0        )/sol_step)); // y is reflected about y=0
    *iz = abs (static_cast<int>((z - region_zmin)/sol_step));
    //printf("pos_to_index: x %lf y %lf z %lf ix %d iy %d iz %d\n",x,y,z,*ix,*iy,*iz);
}
void FieldSol::set_bfld(double x, double y, double z, double bx, double by, double bz)
{
    int i,j,k;
    pos_to_index(x,y,z,&i,&j,&k);
    magfld_bx[i][j][k] = bx;
    magfld_by[i][j][k] = by;
    magfld_bz[i][j][k] = bz;
}
void FieldSol::get_bfld(double x, double y, double z, double *bx, double *by, double *bz)
{
    int i,j,k;
    pos_to_index(x,y,z,&i,&j,&k);
    *bx = magfld_bx[i][j][k];
    *by = magfld_by[i][j][k];
    *bz = magfld_bz[i][j][k];
}

FieldSol::FieldSol(G4String fname)
:sol_step (20.0), 
region_xmin ( 760.0), region_xmax (1240.0), 
region_ymin (-240.0), region_ymax ( 240.0), 
region_zmin (3520.0), region_zmax (4000.0),
file_name(fname), initialised(false)
{
    // Read field map
    init();
}

FieldSol::~FieldSol() {;}

void FieldSol::init()
{
    if (!initialised)
    {
        FILE* fp = fopen(file_name.c_str(),"r");
        char line[514];
        if (fp==NULL) {
            fprintf(stderr,"Magnetic field map %s cannot be opened\n",file_name.c_str());
            return;
        }
        /* map data is defined in global coordinate */
        double in_x,in_y,in_z;
        double in_bx,in_by,in_bz;
        while (fgets(line,sizeof(line),fp)) 
        {
            sscanf(line,"%lf %lf %lf %lf %lf %lf",&in_x,&in_y,&in_z,&in_bx,&in_by,&in_bz);
            set_bfld(in_x,in_y,in_z,in_bx,in_by,in_bz);
        }
        fclose(fp);
        printf("Finish to read solenoid field map\n");
        initialised = true;
    }
    
}

void FieldSol::get_bfield(double x, double y, double z, double *bx, double* by, double *bz)
{
    if (!initialised)
    {
        init();
        if(!initialised) 
        {
            fprintf(stderr,"Magnetic field map %s cannot be opened, exiting\n",file_name.c_str());
            exit(1);
        }
    }
    *bx = 0.0;
    *by = 0.0;
    *bz = 0.0;
    
    if (!((x>=region_xmin && x<=region_xmax) &&
          (y>=region_ymin && y<=region_ymax) &&
          (z>=region_zmin && z<=region_zmax)) )
        return;
    
    int ix,iy,iz;
    pos_to_index(x,y,z,&ix,&iy,&iz);
    double bx0 = magfld_bx[ix][iy][iz];
    double by0 = magfld_by[ix][iy][iz];
    double bz0 = magfld_bz[ix][iy][iz];
    if (y<0) by0 = -by0;
    
    *bx = bx0;
    *by = by0;
    *bz = bz0;
}

void FieldSol::GetFieldValue(const double Point[3],double *Bfield)
{
    Bfield[0] = 0.0;
    Bfield[1] = 0.0;
    Bfield[2] = 0.0;
    
    // double posx = Point[0];
    // double posy = Point[1];
    // double posz = Point[2];
    
    double bx,by,bz;
    
    //get_bfield(posx,posy,posz,&Bfield[0],&Bfield[1],&Bfield[2]); <= wrong !!! no unit  corrected 2011.9.7
    get_bfield(Point[0],Point[1],Point[2],&bx,&by,&bz);
    // get_bfield(posx,posy,posz,&bx,&by,&bz);
    Bfield[0] = bx*tesla;
    Bfield[1] = by*tesla;
    Bfield[2] = bz*tesla;
    
    
    //printf("%lf %lf %lf = %lf %lf %lf\n",posx,posy,posz,Bfield[0],Bfield[1],Bfield[2]);
}

