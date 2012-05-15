// it begins! 
// Messenger for the MuSIC 5 detector construction class

#ifndef DetectorConstructionMessenger_hh
#define DetectorConstructionMessenger_hh 1

#include "globals.hh"
#include "G4UImessenger.hh"

// The class we'll be modifying
class DetectorConstruction;
// Creates the UI directory structure
class G4UIdirectory;
// different types of command
class G4UIcommand;
class G4UIcmdWithAString; // for field file names (possible degrader material)
class G4UIcmdWithADouble; // for dipole polarity & scaling
class G4UIcmdWithADoubleAndUnit; // for scint/degrader thicknesses

class DetectorConstructionMessenger : public G4UImessenger
{
public: 
    DetectorConstructionMessenger(DetectorConstruction*);
    ~DetectorConstructionMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
private:
    
    void init();
    DetectorConstruction* detConstructor_m;
    
    G4UIcommand*   updateCmd_m;
    G4UIdirectory* UIdirectory_m; 
    G4UIcmdWithAString* solenoidFileName_m; // set the filenames for the field maps
    G4UIcmdWithAString* dipoleFileName_m;
    G4UIcmdWithAString* degraderMaterial_m; // set the  degrader material 
    G4UIcmdWithAString* targetMaterial_m; // set the target material 
    G4UIcmdWithADouble* dipolePolarityStrength_m; // scale the dipole strength & polarity
    G4UIcmdWithADoubleAndUnit* scint1Thickness_m; // closer
    G4UIcmdWithADoubleAndUnit* scint2Thickness_m; // further
    G4UIcmdWithADoubleAndUnit* degraderThickness_m;
    G4UIcmdWithADoubleAndUnit* targetThickness_m;
    
    
};

#endif