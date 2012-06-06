#
#
# WARNING
# These functions are _very_ brittle and designed only really to be used
# for MuSIC 5 simulation. 
#
#

# gets the current branch & commit info
# repo info
gitcommit="$(git log --pretty=format:'%h' -n 1)" || gitcommit="not_commited"
gitbranch="$(git symbolic-ref HEAD 2>/dev/null)" || gitbranch="unnamed_branch"
gitbranch=${gitbranch##refs/heads/}

# standard header
header="#\n\
# This is an autogenerated header, do not modify it\n\
# Git commit:\t [$gitbranch $gitcommit]\n\
# Generated on:\t $(date)\n\
#\n"

# common locations etc
# locations etc
outfile_suf="root" # '.' is added at location, mainly for readability
macro_suf="mac"
log_suf="log"

root="../.."
infile="$root/g4blout/monitor6_By-0.04T_cor.root"
exedir="$root/build/Release"
exe="$exedir/music" # usage: ./music <in.root> <out.root> [run.mac]
outdir="$root/output/$runname"
logdir="$outdir/log"
archivedir="$root/output/archive/"

runlog="$logdir/$runname.$log_suf" # master log

# useful functions
archive_dir ()
{ # if the directory $1 exists create a dated & commit numbered archive (in $2), 
# otherwise create the directorys
    if [ ! -d $1 ]; then
        #  if the directories don't exist make them
        echo "Creating new directory: $1"
        mkdir $1
    else 
        # if it does exist make an archive 
        archive_dir="${2}/$(date +"%d-%m-%y_%H%M").$gitcommit"
        echo "Archiving to $archive_dir"
        mkdir $archive_dir
        echo "moving in"
        mv $1 $archive_dir/.
        mkdir $1
    fi
}

make_macro () 
{ # $1 ST material, $2 ST thickness, $3 DegMat $4 DegThickness $5 this filename
    bulk="\
$header\
# Optimising stopping target\n\
#\t STmaterial\t: $1\n\
#\t STthickness\t: $2\n\
#\t DegMaterial\t: $3\n\
#\t DegThickness\t: $4
#\n\
# File name: $5\n\
# \n\n\
/MuSIC_Detector/dipField ../../MuSIC5_detector/fieldmap/fieldmap_dipole.txt\n\
/MuSIC_Detector/solField ../../MuSIC5_detector/fieldmap/fieldmap_solenoid.txt\n\
\n\
/MuSIC_Detector/targetMat $1\n\
/MuSIC_Detector/targetZ $2 mm\n\
/MuSIC_Detector/degraderMat $3\n\
/MuSIC_Detector/degraderZ $4 mm\n\
/run/initialize\n\
"
    echo -e $bulk
}
run_it()
{ # $1 = STmaterial, $2 = STthickness, $3 = DegMaterial $4 = DegThickness $5 = filename
    prefix="$5"
    outfile="$outdir/$prefix.$outfile_suf"
    macfile="$logdir/$prefix.$macro_suf" # save the macros with the logs
    logfile="$logdir/$prefix.$log_suf"
    make_macro $1 $2 $3 $4 $macfile > $macfile
    echo "$5 run started" | tee -a $runlog 
    echo $macfile " generated" >> $runlog
    execmd="$exe $infile $outfile $macfile"
    # for an explanation of the below command see doit.sh
    cmd="( $execmd >> $logfile ) 2>&1 | tee -a $logfile $runlog"
    echo "Running command:" >> $runlog
    echo $cmd >> $runlog
    eval $cmd
    # this will also go to stdout
    echo "$5 run complete" | tee -a $runlog 
    echo -e "************************\n" >> $runlog
}