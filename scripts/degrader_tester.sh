#/usr/bin/bash

# generate macros that set a variety of thicknesses for the MuSIC5 sim
## Git commit hash: [`git branch` `git log --pretty=format:'%h' -n 1`]\n\

# what we want to generate
thicknesses=( 0.2 1 2 5 10 ) # degrader thicknesses in mm
# run with air for each for comparison (curious as to change caused
# by shift in z) ... and because I can't come up with a good way of testing
# air once
materials=( Air Aluminium Polystyrene ) # & Polyethylene?


# locations etc
root="../.."
infile="$root/g4blout/monitor6_By-0T_cor.root"
exedir="$root/build/Release"
exe="$exedir/music" # usage: ./music <in.root> <out.root> [run.mac]
outdir="$root/output"
logdir="$outdir/log"
runlog="$logdir/run.log" # master log

# gets the current branch & commit info
# repo info
gitcommit="$(git log --pretty=format:'%h' -n 1)" || gitcommit="not commited"
gitbranch="$(git symbolic-ref HEAD 2>/dev/null)" || gitbranch="unnamed branch"
gitbranch=${gitbranch##refs/heads/}

# standard header
header="#\n\
# This is an autogenerated header, do not modify it\n\
# Git commit: [$gitbranch $gitcommit]\n\
# Generated on: $(date)\n"

# add the header to the runlog
echo -e $header"# File: $runlog \n" > $runlog

make_macro () 
{
    bulk="\
$header
# File name: $3\n\
# Optimising degrader\n\
#\t material:  $1\n\
#\t thickness: $2\n\
# \n\n\
/MuSIC_Detector/dipField ../../fieldmap/fieldmap_dipole.txt\n\
/MuSIC_Detector/solField ../../fieldmap/fieldmap_solenoid.txt\n\
\n\
/MuSIC_Detector/degraderMat $1\n\
/MuSIC_Detector/degraderZ $2 mm\n\
/run/initialize\n\
"
    echo -e $bulk
}

# record exe status
echo "\`ls -l $exedir\` results in:" >> $runlog
ls -l $exedir >> $runlog 
echo -e "************************\n" >> $runlog

# generate the macros & run the program
for mat in ${materials[@]}; # loop over all entries in the array materials
do
    for thickness in ${thicknesses[@]}; 
    do
        prefix="run_${mat}_${thickness}mm"
        outfile="$outdir/$prefix.root"
        macfile="$logdir/$prefix.mac" # save the macros with the logs
        logfile="$logdir/$prefix.log"
        make_macro $mat $thickness $macfile > $macfile
        echo "$mat $thickness run started" | tee -a $runlog 
        echo $macfile " generated" >> $runlog
        execmd="$exe $infile $outfile $macfile"
                # for an explanation of the below command see doit.sh
        cmd="( $execmd >> $logfile ) 2>&1 | tee -a $logfile $runlog"
        echo "Running command:" >> $runlog
        echo $cmd >> $runlog
        eval $cmd
        # this will also go to stdout
        echo "$mat $thickness run complete" | tee -a $runlog 
        echo -e "************************\n" >> $runlog
    done;
done;