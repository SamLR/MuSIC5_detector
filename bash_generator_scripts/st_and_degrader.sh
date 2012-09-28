#/usr/bin/bash

# ===========================================================================#
# Set things up
# ===========================================================================#
# generate macros that set a variety of thicknesses for the MuSIC5 sim stoping target
# vary both ST thickness and degrader thickness

# What's the name of this run?
runname="st_and_degrader" 
# what we want to generate
STthicknesses=( 1 2 ) # target thicknesses in mm
STmats=( "Copper" "Magnesium" )

degMat="Aluminium"
degThickness=( 1 2 5 8 10 12 ) 

# ===========================================================================#
# Actually do things!
# ===========================================================================#

source common.sh

# save any previous results
# archive_dir $outdir $archivedir
# archive_dir $logdir $archivedir

# add the header to the runlog
echo -e $header"# File: $runlog \n" > $runlog

# record exe status
echo "\`ls -l $exedir\` results in:" >> $runlog
ls -l $exedir >> $runlog 
echo -e "************************\n" >> $runlog

# generate the macros & run the program
for DegZ in ${degThickness[@]}; # loop over all entries in the array materials
do
    for STz in ${STthicknesses[@]}; 
    do
        for STmat in ${STmats[@]};
        do
            name="${runname}_st_${STmat}_${STz}mm_deg_${DegZ}mm"
            echo $DegZ $STz $STmat " => " $name
            # run_it $STmat $STz $degMat $DegZ $name
            echo 
        done;
    done;
done;