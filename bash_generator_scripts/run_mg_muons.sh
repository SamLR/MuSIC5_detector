#!/bin/bash

source common.sh
check_newest

function make_macro () {
	cat > $1 <<-EOM
	# This is a generated macro, DO NOT EDIT
	# 
	# File             : $1
	# Muon charge      : $2
	# target thickness : $4
	# Number of muons  : $3

	# Setting the magnetic field, this shouldn't really need doing but is useful 
	# and stops the hard coded relative links breaking every time you move
	/MuSIC_Detector/dipField /Users/scook/code/MuSIC/simulation/MuSIC_5_detector_sim/MuSIC5/MuSIC5_detector/fieldmap/fieldmap_dipole.txt
	/MuSIC_Detector/solField /Users/scook/code/MuSIC/simulation/MuSIC_5_detector_sim/MuSIC5/MuSIC5_detector/fieldmap/fieldmap_solenoid.txt
	
	/MuSIC_Detector/gun/g4blEnable False
	/gun/particle mu$2
	
	# Set up the degrader
	/MuSIC_Detector/degraderZ 5 mm
	/MuSIC_Detector/degraderMat Air
	/MuSIC_Detector/targetZ $4 mm
	/MuSIC_Detector/targetMat Mg
	
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



targets=( 2.5 5 7.5 15 )
charges=( "+" "-" )
n_muons=100000
outdir="../../output/mg_run"
macrodir="../pid_macros"
# 
# echo "Running all positive muons"
# $exe $infile $outdir/g4bl_mu_pos.root $macrodir/all_pos_mu.mac > /dev/null
# echo "Running all negative muons"
# $exe $infile $outdir/g4bl_mu_neg.root $macrodir/all_neg_mu.mac > /dev/null


for target in "${targets[@]}"; # loop over all entries in the array materials
do
	for charge in "${charges[@]}";
	do
		make_file_name $charge $n_muons $target "Mg"
		macro=$macrodir/$name".mac"
		outfile=$outdir/$name".root"
		make_macro $macro $charge $n_muons $target
		echo "$exe i_dont_exist $outfile $macro > /dev/null"
		$exe i_dont_exist $outfile $macro > /dev/null
	done;
done;
