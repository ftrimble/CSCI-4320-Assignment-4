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
DEBUG=-Wall -DDEBUG_MODE
KRATOS=-DKRATOS
BLUE=-O3 -DBLUE
OPT=-O7
REPORT=findings.aux \
       findings.log \
       findings.pdf
GETDATA=kratosscripts/runscript.sh
MAKEDATA=bluegene/runscript.sh

all: $(OUTPUT)
kratos: $(REPORT) $(OUTPUT)

mm_threaded: src/mm.c
	mpicc $(SOURCES) $(OPT) $(KRATOS) -o $(OUTPUT)

findings.aux: findings.pdf
findings.log: findings.pdf
findings.pdf: bluegene/output
	$(GETDATA)

blue:
	mpicc $(SOURCES) $(BLUE) -o $(OUTPUT)
	$(MAKEDATA)

debug:
	mpicc $(DEBUG) $(OPT) $(KRATOS) $(SOURCES) -o $(OUTPUT)

clean:
	rm *~ $(OUTPUT) $(REPORT)
