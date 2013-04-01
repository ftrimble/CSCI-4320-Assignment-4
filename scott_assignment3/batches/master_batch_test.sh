#!/bin/bash


: ${coresPerNode:=16}

for nodes in 4 8 16 32 64
do
    for ranksPerCore in 1 2 4
    do
        echo batch-$((nodes*coresPerNode))-$((ranksPerCore))-mapping.sh
    done
    echo batch-$((nodes*coresPerNode))-4-mapping.sh
done

