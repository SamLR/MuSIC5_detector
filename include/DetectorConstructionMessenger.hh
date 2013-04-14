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
    // scale the dipole field
    G4UIcmdWithADouble* dipolePolarityStrength_m; // scale the dipole strength & polarity
    // field map file names
    G4UIcmdWithAString* solenoidFileName_m; // set the filenames for the field maps
    G4UIcmdWithAString* dipoleFileName_m;
    // material information
    G4UIcmdWithAString* degraderMaterial_m; // set the  degrader material
    G4UIcmdWithAString* targetMaterial_m; // set the target material 
    // component thicknesses
    G4UIcmdWithADoubleAndUnit* degraderThickness_m;
    G4UIcmdWithADoubleAndUnit* targetThickness_m;
    
    
};

#endif
