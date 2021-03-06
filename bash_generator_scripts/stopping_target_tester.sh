#/usr/bin/bash

# ===========================================================================#
# Set things up
# ===========================================================================#
# generate macros that set a variety of thicknesses for the MuSIC5 sim stoping target

# What's the name of this run?
runname="ST_optimisation" 
# what we want to generate
thicknesses=( 0.2 1 2 5 8 10 ) # target thicknesses in mm
materials=( "Copper" "Magnesium" ) # & Polyethylene?
# materials=( "Aluminium" "Polystyrene" ) # & Polyethylene?

degMat="Aluminium"
degThickness="1"

# ===========================================================================#
# Actually do things!
# ===========================================================================#

source common.sh

# save any previous results
archive_dir $outdir $archivedir
archive_dir $logdir $archivedir

# add the header to the runlog
echo -e $header"# File: $runlog \n" > $runlog

# record exe status
echo "\`ls -l $exedir\` results in:" >> $runlog
ls -l $exedir >> $runlog 
echo -e "************************\n" >> $runlog

echo "Make a basis simulation (1mm Al degrader, no ST)"  | tee -a $runlog 
run_it Air 5 $degMat $degThickness "${runname}_Air_5mm"
echo "Moving to the main loop " | tee -a $runlog 
# generate the macros & run the program
for mat in ${materials[@]}; # loop over all entries in the array materials
do
    for thickness in ${thicknesses[@]}; 
    do
        name="${runname}_${mat}_${thickness}mm"
        run_it $mat $thickness $degMat $degThickness $name
    done;
done;