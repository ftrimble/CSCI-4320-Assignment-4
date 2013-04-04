#!/bin/bash

wd=$(dirname "${BASH_SOURCE[0]}")
rootdir=$wd/..
# raw output from blue gene
indir=$rootdir/bluegene/output
# formatted output for gnuplot
outdir=$rootdir/data
data=(execAvg execMin execMax 
	commAvg commMin commMax 
	mmAvg mmMin mmMax)

rm $outdir/*

for file in `ls -v $indir`; do
	extension=`echo $(basename "$file") | cut -d'.' -f2`
	if [ $extension == "out" ]; then
	    i="0"
	    while read line; do
			if [[ ! -z $line ]]; then 
				echo $line >> $outdir/${data[$i]}.dat
				((i++))
			fi
	    done < $indir/$file
	fi
done
