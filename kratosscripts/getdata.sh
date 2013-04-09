#!/bin/bash

wd=$(dirname $0)
rootdir=$wd/..

# raw output from blue gene
threadsb=$rootdir/output/bluegene/threads
sizeb=$rootdir/output/bluegene/size
# raw output from kratos
threadsk=$rootdir/output/kratos/threads
sizek=$rootdir/output/kratos/size

# formatted output for gnuplot
outroot=$rootdir/data
data=(execAvg execMin execMax 
    size)

#clearing formatted output
rm $outroot/kratos/* $outroot/bluegene/*

for dir in $threadsb $sizeb $threadsk $sizek; do
    if [[ $dir == $threadsk ]] || [[ $dir == $sizek ]]; then outdir=$outroot/kratos
    else outdir=$outroot/bluegene
    fi

    for file in `ls -v $dir`; do
        extension=`echo $(basename "$file") | cut -d'.' -f2`
        if [ $extension == "out" ]; then
            if [[ $dir == $sizek ]] || [[ $dir == $sizeb ]]; then i=3
            else i=0
            fi

	    while read line; do
                if [[ ! -z $line ]]; then
                    echo $line >> $outdir/${data[$i]}.dat
                    ((i++))
	        fi
	    done  < $dir/$file 
        fi
    done
done
