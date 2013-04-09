#!/bin/sh

wd=$(dirname $0)
root=$wd/..

rm $root/output/kratos/threads/* $root/output/kratos/size/*

if [ -d "$root/output/bluegene" ]; then
    if [ ! -d "$root/plots" ]; then
	mkdir $root/plots
    fi
    if [ ! -d "$root/data" ]; then
	mkdir $root/data
	mkdir $root/data/bluegene
	mkdir $root/data/kratos
    fi

    for i in `seq 0 3`; do
        numprocs=`echo 2^$i | bc`
	numthreads=`echo 8/$numprocs | bc`
        echo "Running program with $numprocs processors..."
        mpirun -np $numprocs $root/mm_threaded > \
	    $root/output/kratos/threads/$numthreads\by$numprocs.out
    done

    for i in `seq 1024 1024 4096`; do
	echo "Running program on a matrix of size $i..."
	mpirun -np 2 $root/mm_threaded $i > $root/output/kratos/size/$i.out
    done

    # gets the data for plotting
    $wd/getdata.sh

    # plots the data
    echo "load '$wd/plotscript.p'" | gnuplot

    # refreshes the writeup
    pdflatex $root/findings.tex
    # does it again for proper references
    pdflatex $root/findings.tex

fi


