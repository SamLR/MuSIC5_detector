#! /bin/bash

function write_mac_file {
    # Write the music macro file
    MACRO_DIR=$1  # Where to save the macro
    FIELD_DIR=$2  # Where the field maps for MuSIC are saved
    RUN_LENGTH=$3 # How many particles to simulate
    N=$4          # Which script number this is
    QUEUE=$5      # Which queue is this being submitted to
    FIRST_STEP=$6 # Is there an additional offset?
    # File prefix
    FILE_PRE=${QUEUE}_$N
    # Final output file 
    MACRO_FILE=$MACRO_DIR/$FILE_PRE.mac 
    # Where we should start reading particles from
    OFFSET=$(( $FIRST_STEP+$N*$RUN_LENGTH )) 
    echo "Writing macro file: $FILE_PRE.mac  Length: $RUN_LENGTH  Offset: $OFFSET"
    
    # Use the "here-document" method to write the macro
cat > $MACRO_FILE <<-EOM
#
# This is a generated file! Intended to be run 
# on the $QUEUE queue by qsub
#

# Make sure the defaults are set
/MuSIC_Detector/degraderZ 0 mm
/MuSIC_Detector/targetZ 0.5 mm

# Set the field file locations
/MuSIC_Detector/dipField $FIELD_DIR/fieldmap_dipole.txt
/MuSIC_Detector/solField $FIELD_DIR/fieldmap_solenoid.txt

# Set where to star the run from
/MuSIC_Detector/gun/inputOffset $OFFSET

# Set it all going
/run/initialize
/run/beamOn $RUN_LENGTH
EOM
}

function write_qsub_script {
    
    EXE=$1               # Where the MuSIC program is
    IN_ROOT=$2           # Where the input particles are
    OUT_DIR=$3           # Where the input particles are
    SCRIPT_DIR=$4        # Where are we saving scripts?
    QUEUE=$5             # Which queue are we putting this on?
    N=$6                 # Which script is this?
    
    FILE_PRE=${QUEUE}_$N
    OUT_ROOT=$OUT_DIR/$FILE_PRE.root     # Where are we saving the results
    MAC_FILE=$SCRIPT_DIR/$FILE_PRE.mac   # Which macro are we using
    SCRIPT_FILE=$SCRIPT_DIR/$FILE_PRE.sh # Where should we save the script?
    echo "        Script file: $FILE_PRE.sh"
    # Use the 'here document' method
cat > $SCRIPT_FILE <<EOM
#!/bin/bash
# This is a generated file for use with qsub
$EXE $IN_ROOT $OUT_ROOT $MAC_FILE          
EOM
# Make sure we can run the file
chmod 755 $SCRIPT_FILE
}


# WORK_DIR=/home/scook/MuSIC5_detector
WORK_DIR=/Users/scook/code/MuSIC/simulation/MuSIC_5_detector_sim/MuSIC5/MuSIC5_detector
EXE=$WORK_DIR/WD/bin/Linux-g++/music
IN_ROOT=$WORK_DIR/input/g4bl_out_36_rotation_30435841_particles.root
FIELD_DIR=$WORK_DIR/input
OUT_DIR=$WORK_DIR/output
SCRIPT_DIR=$WORK_DIR/generated_scripts
EMAIL=sam@samlr.com

MID=10  # How many runs do we want on the short queue 
MAX=85 # What's the maximum number of jobs (-1: count from 0)

PICK_UP=0
# do a set of short queue runs 
QUEUE=short # Max 1 hr (sys & wall)
RUN_LENGTH=500 # Assume ~ 10 events/minute = ~50 min (max 1hr)
for (( N=0; N<$MID; N++ ));
do
    echo # blank line
    # Make the macro 
    write_mac_file $SCRIPT_DIR $FIELD_DIR $RUN_LENGTH $N $QUEUE $PICK_UP
    # Make the script for qsub
    write_qsub_script $EXE $IN_ROOT $OUT_DIR $SCRIPT_DIR $QUEUE $N
    echo "        qsub -q $QUEUE -M $EMAIL $SCRIPT_DIR/${QUEUE}_$N.mac"
    # qsub -q $QUEUE -M $EMAIL $SCRIPT_DIR/$N.sh
done

PICK_UP=$(( $RUN_LENGTH*$MID ))
echo "Pick up at: "$PICK_UP
QUEUE=long # Max=72 hours, 96 wall time
RUN_LENGTH=43000 # Assume 10 events/min ~ 72 hours (max)
for (( N=0; N<$(( $MAX-$MID )); N++ ));
do
    echo # blank line
    # Make the macro 
    write_mac_file $SCRIPT_DIR $FIELD_DIR $RUN_LENGTH $N $QUEUE $PICK_UP
    # Make the script for qsub
    write_qsub_script $EXE $IN_ROOT $OUT_DIR $SCRIPT_DIR $QUEUE $N
    echo "        qsub -q $QUEUE -M $EMAIL $SCRIPT_DIR/${QUEUE}_$N.mac"
    # qsub -q $QUEUE -M $EMAIL $SCRIPT_DIR/$N.sh
done
exit 0