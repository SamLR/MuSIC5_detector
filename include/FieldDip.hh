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
#include "G4String.hh"

class G4String;

class FieldDip
{
public:
    FieldDip(G4String fname, double polarity);
    ~FieldDip();
    
    void GetFieldValue( const  double Point[3], double *Bfield );
    void SetFile(G4String &fname) {file_name = fname;};
private:
    
    FieldDip();    

    void init();
    void check_min_max_bounds(double const x, double const y, double const z);
    inline bool valid_position(double x, double y, double z) 
    { 
        return (x > global_xmin) && (x < global_xmax) &&
               (y > global_ymin) && (y < global_ymax) &&
               (z > global_zmin) && (z < global_zmax);  
    };
    
#define MAX_DIP_NX 23
#define MAX_DIP_NY 21
#define MAX_DIP_NZ 51
    double magfld_bx[MAX_DIP_NX][MAX_DIP_NY][MAX_DIP_NZ];
    double magfld_by[MAX_DIP_NX][MAX_DIP_NY][MAX_DIP_NZ];
    double magfld_bz[MAX_DIP_NX][MAX_DIP_NY][MAX_DIP_NZ];
    
    // functions for converting between the co-ordinate systems
    void global_to_center(double gx, double gy, double gz, double& cx, double& cy, double& cz);
    void center_to_global(double cx, double cy, double cz, double& gx, double& gy, double& gz);
    // find the index given co ordinates in the system
    int global_pos_to_index(double x, double y, double z, 
                            unsigned int& ix, unsigned int& iy, unsigned int& iz);
    int centre_pos_to_index(double x, double y, double z, 
                            unsigned int& ix, unsigned int& iy, unsigned int& iz);
    
    // get and set bfield
    void set_bfield(double x, double y, double z, double bx, double by, double bz);
    void get_bfield(double x, double y, double z, double &bx, double& by, double &bz);
    
    
    
    double coil8_x0; //coil8 (last coil of dipole magnet) in global coordinate [mm]
    double coil8_z0;
    double cos36;
    double sin36;
    
    double global_xmin; // solenoid field map region in global coordinate [mm]
    double global_xmax;
    double global_ymin;
    double global_ymax;
    double global_zmin;
    double global_zmax;
    
    double step_size; // step size in mm
    double polarity;
    G4String file_name;
    bool initialised;
};

#endif

