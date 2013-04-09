###############################################################################
###############################################################################
##### Forest Trimble,                   #######################################
##### David Vorick,                     #######################################
##### Scott Todd                        #######################################
##### {trimbf,voricd,todds}@rpi.edu     #######################################
##### Assignment 4:                     #######################################
#####   MPI Matrix Multiply and         #######################################
#####   Performance Analysis with       #######################################
#####   multithreading                  #######################################
##### Due: April 9, 2013                #######################################
###############################################################################
###############################################################################

SOURCES=src/mm.c
OUTPUT=mm_threaded
BLUEOUTDIR=~/data-sb
DEBUG=-Wall -DDEBUG_MODE
KRATOS=-O7 -DKRATOS -pthread
BLUE=-O3 -DBLUE
REPORT=findings.aux \
       findings.log \
       findings.pdf
GETDATA=kratosscripts/runscript.sh
MAKEDATA=sbatch bluegene/batch.slurm

all: $(REPORT)
quick: kratos
kratos: $(OUTPUT) $(REPORT)

mm_threaded: $(SOURCES)
	mpicc $(SOURCES) $(KRATOS) -o $(OUTPUT)

findings.aux: findings.pdf
findings.log: findings.pdf
findings.pdf: bluegene/output
	$(GETDATA)

blue:
	mpicc $(SOURCES) $(BLUE) -o $(BLUEOUTDIR)/$(OUTPUT)
	$(MAKEDATA)

debug:
	mpicc $(DEBUG) $(KRATOS) $(SOURCES) -o $(OUTPUT)

clean:
	rm *~ $(OUTPUT) $(REPORT)
