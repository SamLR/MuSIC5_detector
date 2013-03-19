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

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;  // Physical information on the volume (rotation etc.)
class G4LogicalVolume;    // Logical info (material, detector...)
class G4Material;         // Keep materials at class level for use anywhere
class G4Element;          // Elements
class G4OpticalSurface;   // 2D surfaces rather than further volumes
class Field;              // Magnets
class DetectorConstructionMessenger; // Macro controls
class G4UserLimits;       // Override tracking defaults

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    ~DetectorConstruction();

public:
    G4VPhysicalVolume* Construct();
    
    // inline changes
    void SetSolFieldName(G4String newFile);
    void SetDipFieldName(G4String newFile);

    void SetDegraderZ(const double &newVal) {f_degraderZ = (newVal/2.0); f_updated = true;};
    void SetTargetZ  (const double &newVal) {f_targetZ = (newVal/2.0);   f_updated = true;};

    // these need some amount of checking
    void SetPolarity(double);
    void SetDegraderMat(G4String);
    void SetTargetMat(G4String);
    void UpdateGeometry();
private:
    void DefineMaterials();

    DetectorConstructionMessenger* f_messenger;
    Field* f_myField;// the field map
    
    bool f_updated;

    G4String f_fname_sol; // solenoid fieldmap
    G4String f_fname_dip; // dipole fieldmap
    double f_dip_polarity; // dipole polarity & scaling

    double f_degraderZ; // degreader thickness
    double f_targetZ; // degreader thickness
    
    G4Material* f_scint1Mat;
    G4Material* f_scint2Mat;
    G4Material* f_degraderMat; // degrader material
    G4Material* f_targetMat; // degrader material
    
    G4LogicalVolume*   f_logic_world; // world volume
    G4VPhysicalVolume* f_physi_world;
    
      // pure elements
    G4Material* Pb;
    G4Material* Al;
    G4Material* Mg;
    G4Material* Cu;

      // compounds & mixtures
    G4Material* Air;
    G4Material* Scint;  // Generic scintillator
    G4Material* Mylar;
    G4Material* Polystyrene;
    G4Material* Polyethylene;
    // Specific optical materials
    G4Material* EJ212; // Scintillator
    G4Material* BCF91A_core; // The Wavelength shifting fibre
    G4Material* BCF91A_clad; // Fibre's cladding
    
    // Optical surface (we have two: scintillator/mylar and wls/mylar)
//    G4OpticalSurface* scint_mylar_op_surface;
//    G4OpticalSurface* wls_mylar_op_surface;
    // Assume we can use the same surface for both scintillator & WLS with mylar
    G4OpticalSurface* mylar_surface;
    
    // limits on step length in various materials
    G4UserLimits* f_u_limit;
    G4UserLimits* f_d_limit;
    G4UserLimits* f_deg_limit;
    G4UserLimits* f_st_limit;
};

#endif

