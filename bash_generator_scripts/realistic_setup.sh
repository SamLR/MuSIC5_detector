#/usr/bin/bash

# ===========================================================================#
# Set things up
# ===========================================================================#
# generate macros that set a variety of thicknesses for the MuSIC5 sim stoping target
# vary both ST thickness and degrader thickness

# What's the name of this run?
# runname="final" 
runname="final_alt" 
# What type of targets to use
# the strings will expand at the function call to fill 2 arguments
# i.e. target material & thickness (also note form of loop 
# using `for i in "${thickness[@]}" to expand properly)
# targets=( "Copper 0.5" ) 
targets=( "Magnesium 2.5" "Magnesium 5" "Magnesium 7.5" "Magnesium 10" "Magnesium 12.5" "Magnesium 15" ) 

degMat="Air"
degThickness=( 0 ) 
# degMat="Aluminium"
# degThickness=( 0.5 1 5 8 12 ) 

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
for DegZ in ${degThickness[@]}; # loop over all entries in the array materials
do
    for ST in "${targets[@]}" # "s make bash treat the strings as elements
    do
        make_file_name $runname $ST "Al" $DegZ
        echo $DegZ $ST " => " $name
        if [[ -z $testing ]]; then
            run_it $ST $degMat $DegZ $name
        fi
        echo 
    done;
done;