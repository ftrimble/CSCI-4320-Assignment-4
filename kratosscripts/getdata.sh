#!/bin/bash

wd=$(dirname "${BASH_SOURCE[0]}")
rootdir=$wd/..
# raw output from blue gene
execdir=$rootdir/bluegene/output/threads
sizedir=$rootdir/bluegene/output/size
# formatted output for gnuplot
outdir=$rootdir/data
data=(execAvg execMin execMax 
    sizes)

rm $outdir/*

for dir in $execdir $sizedir; do
    for file in `ls -v $dir`; do
        extension=`echo $(basename "$file") | cut -d'.' -f2`
        if [[ $extension == "out" ]]; then
            if [[ $dir == $execdir ]]; then i="0"
            else i="3"
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
