#/usr/bin/bash

# ===========================================================================#
# Set things up
# ===========================================================================#
# generate macros that set a variety of thicknesses for the MuSIC5 sim stoping target

# What's the name of this run?
runname="degrader_optimisation" 
# what we want to generate
# thicknesses=( 20 50 100 ) # target thicknesses in mm
thicknesses=( 0.2 1 2 5 10 ) # target thicknesses in mm
materials=( "Aluminium" ) # & Polyethylene?
# materials=( "Aluminium" "Polystyrene" ) # & Polyethylene?

STMat="Aluminium"
STThickness="1" 
# STMat="Copper"
# STThickness="6"

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
run_it Air 5 Air 5 "${runname}_Air_5mm"
echo "Moving to the main loop " | tee -a $runlog 
# generate the macros & run the program
for mat in ${materials[@]}; # loop over all entries in the array materials
do
    for thickness in ${thicknesses[@]}; 
    do
        name="${runname}_${mat}_${thickness}mm"
        run_it $STMat $STThickness $mat $thickness $name
    done;
done;