#!/bin/bash

if [ $# -lt 3 ]; then
    echo "./s_loc_tx.sh ip nodeID sourceID"
    exit
else
    ip_addr=$1
    nodeID=$2
    sourceID=$3
    ./s_localization $ip_addr $nodeID $sourceID & 
    matlab -nodisplay -nosplash -nodesktop -r "test_tx;quit;" 2>&1 | tee -a source_loc_matlab_$2.log
fi
