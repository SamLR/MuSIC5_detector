#!/bin/bash

hash=`git log -1 --pretty=format:%h`

# remove the old one
rm music5_sim_*.gz.tar 
tar --disable-copyfile -czvf music5_sim_$hash.gz.tar src/*.cc include/*.hh music.cc
# Copy to UCL 
ssh plus1.hep.ucl.ac.uk "rm MuSIC5_detector/music5_sim_*.gz.tar"
scp music5_sim_$hash.gz.tar plus1.hep.ucl.ac.uk:~/MuSIC5_detector/.
