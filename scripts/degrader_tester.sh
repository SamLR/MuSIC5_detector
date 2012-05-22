#/usr/bin/bash

# ===========================================================================#
# Set things up
# ===========================================================================#
# generate macros that set a variety of thicknesses for the MuSIC5 sim stoping target

# What's the name of this run?
runname="degrader_optimisation" 
# what we want to generate
thicknesses=( 0.2 1 2 5 10 ) # target thicknesses in mm
materials=( "Aluminium" "Polystyrene" ) # & Polyethylene?

# locations etc
outfile_suf="root" # '.' is added at location, mainly for readability
macro_suf="mac"
log_suf="log"

root="../.."
infile="$root/g4blout/monitor6_By-0T_cor.root"
exedir="$root/build/Release"
exe="$exedir/music" # usage: ./music <in.root> <out.root> [run.mac]
outdir="$root/output/$runname"
logdir="$outdir/log"
archivedir="$root/output/archive"

runlog="$logdir/$runname.$log_suf" # master log
STMat="Copper"
STThickness="6"

# ===========================================================================#
# Actually do things!
# ===========================================================================#

source useful_bash.sh

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