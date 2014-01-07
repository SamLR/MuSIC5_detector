#!/bin/bash

function write_script () {
	file_name=$1
	first_event=$2
	last_event=$3
	cat > $file_name <<-'EOM'
	#!/bin/bash
	export PATH=/home/scook/bin:$PATH
	g4bl music_20110622.in firstEvent=$first_event lastEvent=$last_event
	
	EOM
}

# g4bl seems to hit 88 events/s
