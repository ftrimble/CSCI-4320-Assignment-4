### Assignment 4 ###

In this assignment, we are performing a memory constrained matrix 
   multiplication with threading. Here is a bit of information about the 
   layout and some other information:

- src/

  This directory contains the source files, including mm.c which performs the
  matrix multiplication
- plots/ 

  This is an untracked directory which holds the plots that are generated from
  our data.
- data/

  This is an untracked directory, which holds the data gained from running our
  matrix multiplication program.
- findings.tex

  This is the documentation for our results; compile it using make or 
  pdflatex.
- kratos/

  These are a few scripts to help our program run on kratos:
    - plotscript.p

      This file uses gnuplot to turn our data into plots so that we can use
      them in our writeup
    - runscript.sh

      This file sets up our directories and such to support the results of
      mm_threaded. It then runs it according to a smaller configuration that
      is possible on kratos.
- bluegene/

  These are the scripts to run our batches of jobs on blue gene:
    - runscript.sh

      This repeatedly calls the batch script for the necessary 
      configurations.
    - batch.sh

      This actually executes the calls to slurm.