#!/bin/bash

# ===========================================================================#
# Set things up
# ===========================================================================#
# generate macros that set a variety of thicknesses for the MuSIC5 sim stoping target
# vary both ST thickness and degraders thickness

# What's the name of this run?
runname="final" 
targets=( "Copper 0.5" ) 
degraders=( "Air 5" "Aluminium 0.5" "Aluminium 1" "Aluminium 5" ) 

# ===========================================================================#
# Set environment variables
# ===========================================================================#
# This sets the location of the geant4 data files
source /usr/local/bin/geant4.sh

# ===========================================================================#
# Actually do things!
# ===========================================================================#

# common holds all the variables
source common.sh

echo "Checking that we have the latest release of the program"
check_newest

# save any previous results
echo "****archiving****"
archive_dir $outdir $archivedir
archive_dir $logdir $archivedir

# add the header to the runlog
echo -e $header"# File: $runlog \n" > $runlog

# record exe status
echo "\`ls -l $exedir\` results in:" >> $runlog
ls -l $exedir/ >> $runlog 
echo -e "************************\n" >> $runlog

# generate the macros & run the program
for DegZ in "${degraders[@]}"; # loop over all entries in the array materials
do
    for ST in "${targets[@]}" # "s make bash treat the strings as elements
    do
        make_file_name $runname $ST $DegZ
        echo $DegZ $ST " => " $name
        run_it $ST $degMat $DegZ $name
        echo 
    done;
done;