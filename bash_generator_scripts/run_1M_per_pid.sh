#!/bin/bash

function make_macro {
	
	cat > $1 <<- EOM
	# This is an auto generated script
	/MuSIC_Detector/dipField ../fieldmap/fieldmap_dipole.txt
	/MuSIC_Detector/solField ../fieldmap/fieldmap_solenoid.txt

	/control/execute ../pid_macros/pid_$2.mac

	# Toggle using optical processes these are _VERY_ slow
	/MuSIC_Detector/enableOpticalProcesses false

	# Basic command to set it all up and run it. 
	/run/initialize
	/run/beamOn 1000000
	EOM
}
pids=( -211 -13 -11 11 13 211 2212 )
exe="../build/music not_a_file "
macro_dir="../generated_scripts"
outroot_dir="../pid_macros"

for pid in ${pids[@]}; do
	macro_file="$macro_dir/run_$pid.mac"
	outroot_file="$outroot_dir/${pid}_1M.root"
	echo $pid $macro_file $outroot_file
	make_macro $macro_file $pid
	echo $exe $outroot_file $macro_file
	$exe $outroot_file $macro_file
done