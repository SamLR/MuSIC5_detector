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
#include <assert.h>
#include <limits>

using namespace std;
typedef numeric_limits<double> dbl;
typedef numeric_limits<unsigned int> uint;

void FieldSol::index_to_pos(unsigned int ix, unsigned int iy, unsigned int iz,
                            double &x, double &y, double &z)
{
    x = region_xmin + ix*sol_step;
    y = 0.0         + iy*sol_step;
    z = region_xmin + iz*sol_step;
    
}

// change to unsigned?
void FieldSol::pos_to_index(double x, double y, double z, 
                            unsigned int& ix, unsigned int& iy, unsigned int& iz)
{
    ix = static_cast<unsigned int>((x - region_xmin)/sol_step);
    iy = static_cast<unsigned int>((y - 0.0        )/sol_step);
    iz = static_cast<unsigned int>((z - region_zmin)/sol_step);
}

void FieldSol::set_bfield(double x, double y, double z, double bx, double by, double bz)
{
    unsigned int i,j,k;
    pos_to_index(x,y,z,i,j,k);
    magfld_bx[i][j][k] = bx;
    magfld_by[i][j][k] = by;
    magfld_bz[i][j][k] = bz;
}

FieldSol::FieldSol(G4String fname)
:sol_step (20.0), 
region_xmin ( 760.0), region_xmax (1240.0),// FIX ME these should be calculated in init
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
        double in_x=dbl::max(), in_y=dbl::max(), in_z=dbl::max();
        double in_bx=dbl::max(), in_by=dbl::max(), in_bz=dbl::max();
        while (fgets(line,sizeof(line),fp)) 
        {
            sscanf(line,"%lf %lf %lf %lf %lf %lf",&in_x,&in_y,&in_z,&in_bx,&in_by,&in_bz);
            set_bfield(in_x,in_y,in_z,in_bx,in_by,in_bz);
        }
        fclose(fp);
        printf("Finished reading solenoid field map\n");
        initialised = true;
    }
    
}

void FieldSol::get_bfield(double x, double y, double z, double& bx, double& by, double& bz)
{
    if (!initialised) // allows field map to be set late
    {
        init();
        if(!initialised) 
        {
            fprintf(stderr,"Magnetic field map %s cannot be opened, exiting\n",file_name.c_str());
            exit(1);
        }
    }
        
    unsigned int ix=uint::max(), iy=uint::max(), iz=uint::max();
    pos_to_index(x,abs(y),z,ix,iy,iz);
    assert(ix>=0 && iy>=0 && iz>=0);

    bx = magfld_bx[ix][iy][iz];
    by = magfld_by[ix][iy][iz];
    bz = magfld_bz[ix][iy][iz];
    
    if (y < 0) by = -by;
}

void FieldSol::GetFieldValue(const double Point[3],double *Bfield)
{
    if (not valid_position(Point[0],Point[1],Point[2])) return;
    
    double bx=dbl::max(),by=dbl::max(),bz=dbl::max();

    get_bfield(Point[0],Point[1],Point[2],bx,by,bz);

    Bfield[0] = bx*tesla;
    Bfield[1] = by*tesla;
    Bfield[2] = bz*tesla;
}

