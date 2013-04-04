#!/bin/sh

wd=$(dirname "${BASH_SOURCE[0]}")
root=$wd/..

if [ -d "$root/bluegene/output" ]; then

	if [ ! -d "$root/plots" ]; then
		mkdir $root/plots
	fi

	if [ ! -d "$root/data" ]; then
		mkdir $root/data
	fi

	# gets the data for plotting
	$wd/getdata.sh

	# plots the data
	echo "load $wd/plotscript.p" | gnuplot

	# refreshes the writeup
	pdflatex $root/findings.tex
fi


