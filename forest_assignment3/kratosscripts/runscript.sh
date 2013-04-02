#!/bin/bash

dir=$(dirname "${BASH_SOURCE[0]}")
rootdir=$dir/..

if [ ! -d "plots" ]; then
    mkdir $rootdir/plots
    mkdir $rootdir/plots/bluegene
    mkdir $rootdir/plots/bluegene/experiment1
    mkdir $rootdir/plots/bluegene/experiment2
fi

if [ -z $1 ]; then
    if [ ! -d "$rootdir/data" ]; then
	mkdir $rootdir/data
    fi

# clears data before rerunning program
    rm $rootdir/data/*.dat

# runs program on all numbers of processors.
    for i in `seq 8 8 16`; do
	echo "Running program with " $i " processors"
	mpirun -np $i $rootdir/trimbf_mm > $i\_procs.out
    done
elif [ $1 == "QUICK" ]; then
    echo "Running smaller program with 8 processors"
    mpirun -np 8 $rootdir/trimbf_mm > quick.out
fi

$dir/getdata.sh

