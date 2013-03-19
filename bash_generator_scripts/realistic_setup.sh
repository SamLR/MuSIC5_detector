#/usr/bin/bash

# ===========================================================================#
# Set things up
# ===========================================================================#
# generate macros that set a variety of thicknesses for the MuSIC5 sim stoping target
# vary both ST thickness and degrader thickness

# What's the name of this run?
runname="final" 
# runname="final_alt" 
# What type of targets to use
# the strings will expand at the function call to fill 2 arguments
# i.e. target material & thickness (also note form of loop 
# using `for i in "${thickness[@]}" to expand properly)
targets=( "Copper 0.5" ) 
# targets=( "Magnesium 2.5" "Magnesium 5" "Magnesium 7.5" "Magnesium 10" "Magnesium 12.5" "Magnesium 15" ) 

# degMat="Air"
# degThickness=( 0 ) 
# degMat=( "Aluminium" )
degThickness=( "Air 5" "Aluminium 0.5" "Aluminium 1" "Aluminium 5" "Aluminium 8" "Aluminium 12" ) 

# ===========================================================================#
# Set environment variables
# ===========================================================================#

export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:/usr/local/lib"
export G4ABLADATA="/usr/local/share/Geant4-9.5.0/data/G4ABLA3.0"
export G4LEVELGAMMADATA="/usr/local/share/Geant4-9.5.0/data/PhotonEvaporation2.2"
export G4LEDATA="/usr/local/share/Geant4-9.5.0/data/G4EMLOW6.23"
export G4NEUTRONHPDATA="/usr/local/share/Geant4-9.5.0/data/G4NDL4.0"
export G4NEUTRONXSDATA="/usr/local/share/Geant4-9.5.0/data/G4NEUTRONXS1.1"
export G4PIIDATA="/usr/local/share/Geant4-9.5.0/data/G4PII1.3"
export G4RADIOACTIVEDATA="/usr/local/share/Geant4-9.5.0/data/RadioactiveDecay3.4"
export G4REALSURFACEDATA="/usr/local/share/Geant4-9.5.0/data/RealSurface1.0"

# ===========================================================================#
# Actually do things!
# ===========================================================================#

source common.sh
testing=""
# save any previous results
if [[ -z $testing ]]; then
    echo "****archiving****"
    archive_dir $outdir $archivedir
    archive_dir $logdir $archivedir
fi

# add the header to the runlog
echo -e $header"# File: $runlog \n" > $runlog

# record exe status
echo "\`ls -l $exedir\` results in:" >> $runlog
ls -l $exedir >> $runlog 
echo -e "************************\n" >> $runlog

# generate the macros & run the program
for DegZ in "${degThickness[@]}"; # loop over all entries in the array materials
do
    for ST in "${targets[@]}" # "s make bash treat the strings as elements
    do
        make_file_name $runname $ST $DegZ
        echo $DegZ $ST " => " $name
        if [[ -z $testing ]]; then
            run_it $ST $degMat $DegZ $name
        fi
        echo 
    done;
done;