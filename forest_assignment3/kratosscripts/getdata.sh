#!/bin/bash

wd=$(dirname "${BASH_SOURCE[0]}")
rootdir=$wd/..
dir="$rootdir/data/bluegene"

for j in `seq 1 2`; do 
    expdir=$dir/experiment$j
    rm $expdir/*
    tar -xzf $dir/exp$j.tar.gz -C $expdir

    # ensures that files are sorted by number
    # of tasks, allowing for much nicer graphs.
    files=`ls $expdir`
    for file in $files; do
	name=`echo $(basename "$file") | cut -d'.' -f1`
	tasksperproc=`echo $name | cut -d'b' -f1`
	numprocs=`echo $name | cut -d'y' -f2`
	numtasks=`echo "$tasksperproc*$numprocs" | bc`
	mv $expdir/$file $expdir/$numtasks\_$file
    done

    for file in `ls -v $expdir`; do
	filename=$(basename "$file")
	extension=`echo "$filename" | cut -d'.' -f2`
	if [ $extension == "out" ]; then
	    i="0"
	    while read line; do
		if [[ ! -z $line ]]; then 
		    echo $line >> $i
		    i=`echo $i+1 | bc`
		fi
	    done < $expdir/$file
	fi
    done

    cat 0 >> $expdir/execAvg.dat
    cat 1 >> $expdir/execMin.dat
    cat 2 >> $expdir/execMax.dat
    cat 3 >> $expdir/commAvg.dat
    cat 4 >> $expdir/commMax.dat
    cat 5 >> $expdir/commMin.dat
    cat 6 >> $expdir/mmAvg.dat
    cat 7 >> $expdir/mmMin.dat
    cat 8 >> $expdir/mmMax.dat

    for i in `seq 0 8`; do
	rm $i
    done
    
    rm $expdir/*.out
done 
