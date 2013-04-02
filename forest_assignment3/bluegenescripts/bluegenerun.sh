#!/bin/sh

for i in `seq 6 10`; do
    numprocs=`echo "2^$i" | bc`
    numnodes=`echo "2^($i-4)" | bc`
    sbatch --nodes=$numnodes experiment1.sh $numprocs $numnodes
    sbatch --nodes=$numnodes experiment2.sh $numprocs $numnodes
done