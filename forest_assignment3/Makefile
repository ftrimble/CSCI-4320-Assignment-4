# these two rules regenerate the binary and the plots.
all:
	mpicc -O7 -DKRATOS src/mm.c -o trimbf_mm
	kratosscripts/runscript.sh NONEWDATA
	echo "load 'kratosscripts/plotscript.p'" | gnuplot 
	pdflatex findings.tex

kratos: 
	mpicc -O7 -DKRATOS src/mm.c -o trimbf_mm
	kratosscripts/runscript.sh NONEWDATA
	echo "load 'kratosscripts/plotscript.p'" | gnuplot 
	pdflatex findings.tex


# This rule actually generates the data on kratos
run:
	mpicc -O7 -DKRATOS src/mm.c -o trimbf_mm
	kratosscripts/runscript.sh
	echo "load 'kratosscripts/plotscript.p'" | gnuplot
	pdflatex findings.tex

# This rule runs the program on a smaller 
# scale to show that it is possible.
quick: 
	mpicc -O7 -DKRATOS -DQUICK src/mm.c -o trimbf_mm
	kratosscripts/runscript.sh QUICK

# compiles and runs on the blue gene
blue: 
	mpicc -O3 mm.c -o ~/data-sb/trimbf_mm
	bluegenescripts/bluegenerun.sh

# runs on kratos with an additional 
# -DDEBUG option for debugging purposes.
debug: 
	mpicc -O7 -Wall -DDEBUG_MODE -DKRATOS mm.c -o trimbf_mm
	kratosscripts/runscript.sh
	echo "load 'kratosscripts/plotscript.p'" | gnuplot 
	pdflatex findings.tex

# removes intermediate files and final results
clean: 
	rm -r plots findings.aux findings.log findings.pdf trimbf_mm \
	*~ src/*~ kratosscripts/*~ bluegenescripts/*~
