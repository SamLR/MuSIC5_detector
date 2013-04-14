// it begins


#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"

#include "G4UIdirectory.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* dc)
: detConstructor_m(dc), updateCmd_m(NULL), UIdirectory_m(NULL),  
dipolePolarityStrength_m(NULL), solenoidFileName_m(NULL), dipoleFileName_m(NULL), 
degraderMaterial_m(NULL), targetMaterial_m(NULL),
degraderThickness_m(NULL), targetThickness_m(NULL)
{
    init();
}

DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
    delete updateCmd_m;
    delete UIdirectory_m; 
    delete dipolePolarityStrength_m;
    delete solenoidFileName_m;
    delete dipoleFileName_m;
    delete degraderMaterial_m;
    delete targetMaterial_m;  
    delete degraderThickness_m;
    delete targetThickness_m;
}

void DetectorConstructionMessenger::init()
{
    // set up control the structure
    UIdirectory_m = new G4UIdirectory("/MuSIC_Detector/"); 
    UIdirectory_m->SetGuidance("Specifies options for the Detector");

    // solenoid fieldmap name
    solenoidFileName_m = new G4UIcmdWithAString("/MuSIC_Detector/solField", this); 
    solenoidFileName_m->SetGuidance("Specify the fieldmap to be used for the solenoid");
    solenoidFileName_m->SetDefaultValue("../../MuSIC5_detector/fieldmap/fieldmap_solenoid.txt");

    // dipole fieldmap name & polarity 
    dipoleFileName_m = new G4UIcmdWithAString("/MuSIC_Detector/dipField", this);
    dipoleFileName_m->SetGuidance("Specify the fieldmap to be used for the dipole");
    dipoleFileName_m->SetDefaultValue("../../MuSIC5_detector/fieldmap/fieldmap_dipole.txt");

    dipolePolarityStrength_m = new G4UIcmdWithADouble("/MuSIC_Detector/polarity", this);
    dipolePolarityStrength_m->SetGuidance("Scale the dipole field by some amount x (-1 < x < 1)");
    dipolePolarityStrength_m->SetParameterName("polarity", true);
    dipolePolarityStrength_m->SetDefaultValue(1.0);

    // degrader thickness & material
    degraderThickness_m = new G4UIcmdWithADoubleAndUnit("/MuSIC_Detector/degraderZ", this); 
    degraderThickness_m->SetGuidance("Thickness of the degrader");
    degraderThickness_m->SetParameterName("degraderZ", true);
    degraderThickness_m->SetDefaultValue(10.0);
    degraderThickness_m->SetDefaultUnit("mm");

    degraderMaterial_m = new G4UIcmdWithAString("/MuSIC_Detector/degraderMat", this); 
    degraderMaterial_m->SetGuidance("Set the material to make the degrader from");
    degraderMaterial_m->SetDefaultValue("Polystyrene");
    
    // target thickness & material
    targetThickness_m = new G4UIcmdWithADoubleAndUnit("/MuSIC_Detector/targetZ", this); 
    targetThickness_m->SetGuidance("Thickness of the target");
    targetThickness_m->SetParameterName("targetZ", true);
    targetThickness_m->SetDefaultValue(10.0);
    targetThickness_m->SetDefaultUnit("mm");

    targetMaterial_m = new G4UIcmdWithAString("/MuSIC_Detector/targetMat", this); 
    targetMaterial_m->SetGuidance("Set the material to make the target from");
    targetMaterial_m->SetDefaultValue("Cu");
    
    updateCmd_m = new G4UIcommand("/MuSIC_Detector/update",this);
    updateCmd_m->SetGuidance("Update the detector geometry with changed values.");
    updateCmd_m->SetGuidance("Must be run before beamOn if detector has been changed.");
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if ( command == solenoidFileName_m)
    {
        detConstructor_m->SetSolFieldName(newValue);
    }
    else if ( command == dipoleFileName_m)
    {
        detConstructor_m->SetDipFieldName(newValue);
    }
    else if ( command == dipolePolarityStrength_m)
    {
        detConstructor_m->SetPolarity(dipolePolarityStrength_m->GetNewDoubleValue(newValue));
    }
    else if ( command == degraderThickness_m)
    {
        detConstructor_m->SetDegraderZ(degraderThickness_m->GetNewDoubleValue(newValue));
    }  
    else if ( command == degraderMaterial_m)
    {
        detConstructor_m->SetDegraderMat(newValue);
    }
    else if ( command == targetThickness_m)
    {
        detConstructor_m->SetTargetZ(targetThickness_m->GetNewDoubleValue(newValue));
    }  
    else if ( command == targetMaterial_m)
    {
        detConstructor_m->SetTargetMat(newValue);
    } 
    else if ( command == updateCmd_m)
    {
        detConstructor_m->UpdateGeometry();
    }

}
