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
	/run/beamOn $3
	EOM
}
pids=( -13 13 -11 11 211 -211 2212 )
exe="../build/music not_a_file "
macro_dir="../generated_scripts"
outroot_dir="../pid_macros/root"

if [[ $1 ]]; then
	n_events=$1
else
	n_events=10000
fi 

for pid in ${pids[@]}; do
	macro_file="$macro_dir/run_$pid.mac"
	outroot_file="$outroot_dir/${pid}_${n_events}.root"
	echo $pid $macro_file $outroot_file $n_events
	make_macro $macro_file $pid $n_events
	echo $exe $outroot_file $macro_file
	$exe $outroot_file $macro_file > /dev/null
done