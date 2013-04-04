#!/bin/bash

wd=$(dirname "${BASH_SOURCE[0]}")
dir="$rootdir/data/bluegene"
data=(execAvg execMin execMax 
	commAvg commMin commMax 
	mmAvg mmMin mmMax)

for file in `ls -v $dir`; do
	extension=`echo $(basename "$file") | cut -d'.' -f2`
	if [ $extension == "out" ]; then
	    i="0"
	    while read line; do
			if [[ ! -z $line ]]; then 
				echo $line >> $dir/data[$i].dat
				((i++))
			fi
	    done < $dir/$file
	fi
done
