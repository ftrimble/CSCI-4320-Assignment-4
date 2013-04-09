#!/bin/bash

wd=$(dirname $0)
rootdir=$wd/..
# raw output from blue gene
execdir=$rootdir/output/bluegene/threads
sizedir=$rootdir/output/bluegene/size
kratosdir=$rootdir/output/kratos/
# formatted output for gnuplot
outroot=$rootdir/data
data=(execAvg execMin execMax 
    size)

rm $outroot/kratos/* $outroot/bluegene/*

for dir in $execdir $sizedir $kratosdir; do
    if [[ $dir == $kratosdir ]]; then outdir=$outroot/kratos
    else outdir=$outroot/bluegene
    fi

    for file in `ls -v $dir`; do
        extension=`echo $(basename "$file") | cut -d'.' -f2`
        if [[ $extension == "out" ]]; then
            if [[ $dir == $sizedir ]]; then i=3
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
