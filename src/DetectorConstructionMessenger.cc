// it begins


#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"

#include "G4UIdirectory.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* dc)
    : detConstructor_m(dc)
{
    init();
}

DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
    delete UIdirectory_m; 
    delete solenoidFileName_m;
    delete dipoleFileName_m;
    delete degraderMaterial_m;
    delete dipolePolarityStrength_m;
    delete scint1Thickness_m; 
    delete scint2Thickness_m; 
    delete degraderThickness_m;
}

void DetectorConstructionMessenger::init()
{
    // set up control the structure
    UIdirectory_m = new G4UIdirectory("/MuSIC_Detector/"); 
    UIdirectory_m->SetGuidance("Specifies options for the Detector");

    // solenoid fieldmap name
    solenoidFileName_m = new G4UIcmdWithAString("/MuSIC_Detector/solField", this); 
    solenoidFileName_m->SetGuidance("Specify the fieldmap to be used for the solenoid");
    solenoidFileName_m->SetDefaultValue("fieldmap/fieldmap_solenoid.txt");
    solenoidFileName_m->AvailableForStates(G4State_PreInit,G4State_Idle);

    // dipole fieldmap name & polarity 
    dipoleFileName_m = new G4UIcmdWithAString("/MuSIC_Detector/dipField", this);
    dipoleFileName_m->SetGuidance("Specify the fieldmap to be used for the dipole");
    dipoleFileName_m->SetDefaultValue("fieldmap/fieldmap_dipole.txt");
    dipoleFileName_m->AvailableForStates(G4State_PreInit,G4State_Idle);

    dipolePolarityStrength_m = new G4UIcmdWithADouble("/MuSIC_Detector/polarity", this);
    dipolePolarityStrength_m->SetGuidance("Scale the dipole field by some amount x (-1 < x < 1)");
    dipolePolarityStrength_m->SetParameterName("polarity", true);
    dipolePolarityStrength_m->SetDefaultValue(1.0);
    // dipolePolarityStrength_m->AvailableForStates(G4State_PreInit,G4State_Idle);

    // scintillator 1's thickness (closer of the two)
    scint1Thickness_m = new G4UIcmdWithADoubleAndUnit("/MuSIC_Detector/scint1z", this); // closer
    scint1Thickness_m->SetGuidance("Thickness of scintillator 1 (nearest to beam)");
    scint1Thickness_m->SetParameterName("scint1z", true);
    scint1Thickness_m->SetDefaultValue(1.0);
    scint1Thickness_m->SetDefaultUnit("mm");
    scint1Thickness_m->AvailableForStates(G4State_PreInit,G4State_Idle);

    // scintillator 2's thickness
    scint2Thickness_m = new G4UIcmdWithADoubleAndUnit("/MuSIC_Detector/scint2z", this); // further
    scint2Thickness_m->SetGuidance("Thickness of scintillator 2 (furthest from beam)");
    scint2Thickness_m->SetParameterName("scint2z", true);
    scint2Thickness_m->SetDefaultValue(20.0);
    scint2Thickness_m->SetDefaultUnit("mm");
    scint2Thickness_m->AvailableForStates(G4State_PreInit,G4State_Idle);

    // degrader thickness & material
    degraderThickness_m = new G4UIcmdWithADoubleAndUnit("/MuSIC_Detector/degraderZ", this); 
    degraderThickness_m->SetGuidance("Thickness of the degrader");
    degraderThickness_m->SetParameterName("degraderZ", true);
    degraderThickness_m->SetDefaultValue(10.0);
    degraderThickness_m->SetDefaultUnit("mm");
    degraderThickness_m->AvailableForStates(G4State_PreInit,G4State_Idle);

    degraderMaterial_m = new G4UIcmdWithAString("/MuSIC_Detector/degragerMat", this); 
    degraderMaterial_m->SetGuidance("Set the material to make the degrader from");
    degraderMaterial_m->SetDefaultValue("Pb");
    degraderMaterial_m->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    // target thickness & material
    targetThickness_m = new G4UIcmdWithADoubleAndUnit("/MuSIC_Detector/targetZ", this); 
    targetThickness_m->SetGuidance("Thickness of the target");
    targetThickness_m->SetParameterName("targetZ", true);
    targetThickness_m->SetDefaultValue(10.0);
    targetThickness_m->SetDefaultUnit("mm");
    targetThickness_m->AvailableForStates(G4State_PreInit,G4State_Idle);

    targetMaterial_m = new G4UIcmdWithAString("/MuSIC_Detector/targetMat", this); 
    targetMaterial_m->SetGuidance("Set the material to make the target from");
    targetMaterial_m->SetDefaultValue("Cu");
    targetMaterial_m->AvailableForStates(G4State_PreInit,G4State_Idle);
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
    else if ( command == scint1Thickness_m)
    {
        detConstructor_m->SetScint1z(scint1Thickness_m->GetNewDoubleValue(newValue));
    } 
    else if ( command == scint2Thickness_m)
    {
        detConstructor_m->SetScint2z(scint2Thickness_m->GetNewDoubleValue(newValue));
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

}