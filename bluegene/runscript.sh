#!/bin/sh

numnodes=1
dir=$(dirname "${BASH_SOURCE[0]}")

for i in `seq 0 6`; do
    numtasks=`echo "2^$i" | bc`
    numthreads=`echo "64/$numtasks" | bc`
    sbatch --nodes=$numnodes $dir/batch.sh $numtasks $numthreads
done
