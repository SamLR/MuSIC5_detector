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
#include <limits>

using namespace std;
typedef numeric_limits<double> dbl_limits;
typedef numeric_limits<unsigned int> uint_limits;

void set_quadrant_polarity(int quadrant, double& sx, double& sy, double& sz) 
{
    sx = 0; sy = 0; sz = 0;
    switch (abs(quadrant)) 
    {
        case 1:
            sx = 1.0;
            sy = 1.0;
            sz = 1.0; 
            return;
        case 2:
            sx =-1.0;
            sy = 1.0;
            sz = 1.0; 
            return;
        case 3:
            sx =-1.0;
            sy = 1.0;
            sz =-1.0;
            return;  
        case 4:
            sx = 1.0;
            sy = 1.0; 
            sz =-1.0; 
            return;
        default:
            return;
    }
}

void FieldDip::check_min_max_bounds(double const x, double const y, double const z)
{
    // check & set minimum extents
    if (x < global_xmin) global_xmin = x;
    if (y < global_ymin) global_ymin = y;
    if (z < global_zmin) global_zmin = z;
    // check & set maximum extents
    if (x > global_xmax) global_xmax = x;
    if (y > global_ymax) global_ymax = y;
    if (z > global_zmax) global_zmax = z;
}

void FieldDip::global_to_center(double gx, double gy, double gz, double& cx, double& cy, double& cz)
{
    cy = gy;
    cx = (gx-coil8_x0)*cos36 - (gz-coil8_z0)*sin36;
    cz = (gx-coil8_x0)*sin36 + (gz-coil8_z0)*cos36;
}


void FieldDip::center_to_global(double cx, double cy, double cz, double& gx, double& gy, double& gz)
{
    gy = cy;
    gx = cx*cos36 + cz*sin36 + coil8_x0;
    gz = cz*cos36 - cx*sin36 + coil8_z0;
}

int FieldDip::centre_pos_to_index(double x, double y, double z, 
                                  unsigned int& ix, unsigned int& iy, unsigned int& iz)
{
    int quadrant = 0;
    int ix0 = static_cast<int>( x/step_size );
    int iy0 = static_cast<int>( y/step_size );
    int iz0 = static_cast<int>( z/step_size );
    
    if (abs(ix0) >= MAX_DIP_NX || abs(iy0) >= MAX_DIP_NY || abs(iz0) >= MAX_DIP_NZ) quadrant = 0;    
    else if (ix0>0 && iz0>0 && iy0>0) quadrant = 1;
    else if (ix0<0 && iz0>0 && iy0>0) quadrant = 2;
    else if (ix0<0 && iz0<0 && iy0>0) quadrant = 3;
    else if (ix0>0 && iz0<0 && iy0>0) quadrant = 4;
    else if (ix0>0 && iz0>0 && iy0<0) quadrant = -1;
    else if (ix0<0 && iz0>0 && iy0<0) quadrant = -2;
    else if (ix0<0 && iz0<0 && iy0<0) quadrant = -3;
    else if (ix0>0 && iz0<0 && iy0<0) quadrant = -4;
    
    ix = abs(ix0);
    iy = abs(iy0);
    iz = abs(iz0);
    
    return quadrant;
}

int FieldDip::global_pos_to_index(double x, double y, double z, 
                                  unsigned int& ix, unsigned int& iy, unsigned int& iz)
{
    double cx=dbl_limits::max(), cy=dbl_limits::max(), cz=dbl_limits::max();
    global_to_center(x,y,z,cx,cy,cz);
    return centre_pos_to_index(cx,cy,cz,ix,iy,iz);
}

void FieldDip::set_bfield(double x, double y, double z, double bx, double by, double bz)
{
    unsigned int i=uint_limits::max(),j=uint_limits::max(),k=uint_limits::max();
    centre_pos_to_index(x,y,z,i,j,k);
    
    if (i >= MAX_DIP_NX || j >= MAX_DIP_NY || k >= MAX_DIP_NZ) {
        fprintf(stderr,"ERROR: FieldDip::set_bfld overflowed x=%lf y=%lf z=%lf\n",x,y,z);
        exit(1);
    }
    
    magfld_bx[i][j][k] = bx;
    magfld_by[i][j][k] = by;
    magfld_bz[i][j][k] = bz;
}


FieldDip::FieldDip(G4String fname, int polarity)
: coil8_x0 (776.3),      coil8_z0 (3420.1),       // set the dipole origin
cos36(0.809016994),      sin36(0.587785252),      // precalc useful numbers 
global_xmin(dbl_limits::max()), global_xmax(dbl_limits::min()), // set for absolute min&max
global_ymin(dbl_limits::max()), global_ymax(dbl_limits::min()), 
global_zmin(dbl_limits::max()), global_zmax(dbl_limits::min()),
step_size(10.0),         polarity(polarity),      
file_name(fname),        initialised(false)
{
    init();
}

FieldDip::FieldDip()
{ 
    FieldDip::FieldDip(G4String("ERROR: NO FILE SPECIFIED"), 0);
}

void FieldDip::init()
{
    if (!initialised) 
    {
        // Read field map
        FILE* fp = fopen(file_name.c_str(),"r");
        char line[514];
        if (fp==NULL) {
            fprintf(stderr,"ERROR: failed to open dipole field map %s\n",file_name.c_str());
            return;
        }
        /* map data is defined in centerline coordinate */
        double in_x=dbl_limits::max(), in_y=dbl_limits::max(), in_z=dbl_limits::max(); // make errors obvious
        double in_bx=dbl_limits::max(), in_by=dbl_limits::max(), in_bz=dbl_limits::max();
        double in_ba=dbl_limits::max();
        
        while (fgets(line,sizeof(line),fp)) {
            sscanf(line,"%lf %lf %lf %lg %lg %lg %lg",&in_x,&in_y,&in_z,&in_bx,&in_by,&in_bz,&in_ba);
            
            set_bfield(in_x,in_y,in_z,
                     in_bx*polarity,
                     in_by*polarity,
                     in_bz*polarity);
            
            check_min_max_bounds(in_x, in_y, in_z);
            
        }
        fclose(fp);
        if (global_xmin == 0) global_xmin = -global_xmax; // assume 0 values => symetric
        if (global_ymin == 0) global_ymin = -global_ymax;
        if (global_zmax == 0) global_zmin = -global_zmax;
        
        // actually make the mins/maxs global
        center_to_global(global_xmin, global_ymin, global_zmin, 
                         global_xmin, global_ymin, global_zmin);
        center_to_global(global_xmax, global_ymax, global_zmax, 
                         global_xmax, global_ymax, global_zmax);
        
        printf("Finished reading dipole field map\n");
        initialised = true;
    }
}

FieldDip::~FieldDip()
{;}

void FieldDip::get_bfield(double x, double y, double z, double& bx, double& by, double& bz)
{
    if(!initialised)
    {
        init();
        if (!initialised) 
        {
            fprintf(stderr,"ERROR: failed to open dipole field map, exiting %s\n",file_name.c_str());
            exit(1);
        }
    }
    
    // dipole only defined in positive quadrant, it is symmetric though
    // make this always break if indecies are not assigned elsewhere
    unsigned int ix=uint_l::max(), iy=uint_l::max(), iz=uint_l::max();
    double sx=0, sy=0, sz=0;
    int quadrant = global_pos_to_index(x,y,z,ix,iy,iz);
    
    // quadrant of 0 means that the particle is outside the area defined in the fieldmap
    if (quadrant == 0)
    {
        bx = 0.0;
        by = 0.0;
        bz = 0.0;
    } else 
    {
        set_quadrant_polarity(quadrant, sx, sy, sz);
        
        bx = sx*magfld_bx[ix][iy][iz];
        by = sy*magfld_by[ix][iy][iz];
        bz = sz*magfld_bz[ix][iy][iz];   
    }
}

void FieldDip::GetFieldValue(const double Point[3],double *Bfield)
{
    if (not valid_position(Point[0],Point[1],Point[2])) return;
    
    double bx = dbl_limits::max(), by = dbl_limits::max(), bz = dbl_limits::max();
    
    get_bfield(Point[0],Point[1],Point[2],bx,by,bz);
    
    Bfield[0] = bx*tesla;
    Bfield[1] = by*tesla;
    Bfield[2] = bz*tesla;
    
}





