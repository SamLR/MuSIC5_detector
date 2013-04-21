#!/bin/bash

source common.sh
check_newest

function make_macro () {
	cat > $1 <<-EOM
	# This is a generated macro, DO NOT EDIT
	# 
	# File               : $1
	# Muon charge        : $2
	# Degrader thickness : $4
	# Degrader material  : $5
	# Number of muons    : $3

	# Setting the magnetic field, this shouldn't really need doing but is useful 
	# and stops the hard coded relative links breaking every time you move
	/MuSIC_Detector/dipField /Users/scook/code/MuSIC/simulation/MuSIC_5_detector_sim/MuSIC5/MuSIC5_detector/fieldmap/fieldmap_dipole.txt
	/MuSIC_Detector/solField /Users/scook/code/MuSIC/simulation/MuSIC_5_detector_sim/MuSIC5/MuSIC5_detector/fieldmap/fieldmap_solenoid.txt
	
	# Enable G4BL and select 'PID-only' mode, select positive muons
	/MuSIC_Detector/gun/g4blEnable true
	/MuSIC_Detector/gun/g4blSinglePID true
	/gun/particle mu$2
	
	# Set up the degrader
	/MuSIC_Detector/degraderZ $4 mm
	/MuSIC_Detector/degraderMat $5
	
	# Basic command to set it all up and run it. 
	/MuSIC_Detector/update
	/run/initialize
	
	# Run all the muons we have
	/run/beamOn $3
	EOM
}

function make_file_name () {
	# $1: charge, $2: n_muons, $3:degraderZ, $4: degraderMat
	name="mu$1_$3mm_$4_$2"
}



degraders=( "5 Air" "0.5 Aluminium" "1 Aluminium" "5 Aluminium" ) 
charges=( "+ 86710" "- 9009" )
# n_muons=(   )

outdir="../../output/mu+_mu_ratio"
macrodir="../pid_macros"
# 
# echo "Running all positive muons"
# $exe $infile $outdir/g4bl_mu_pos.root $macrodir/all_pos_mu.mac > /dev/null
# echo "Running all negative muons"
# $exe $infile $outdir/g4bl_mu_neg.root $macrodir/all_neg_mu.mac > /dev/null


for degrader in "${degraders[@]}"; # loop over all entries in the array materials
do
	for charge in "${charges[@]}";
	do
		make_file_name $charge $degrader
		macro=$macrodir/$name".mac"
		outfile=$outdir/$name".root"
		make_macro $macro $charge $degrader
		echo "$exe $infile $outfile $macro > /dev/null"
		$exe $infile $outfile $macro > /dev/null
	done;
done;
