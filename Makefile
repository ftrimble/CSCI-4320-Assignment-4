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
BLUE=-DBLUE
QUICK=-DQUICK
OPT=-O7
REPORT=findings.aux \
       findings.log \
       findings.pdf

blue:
	mpicc -O3 $(SOURCES) $(BLUE) -o $(OUTPUT)
kratos:
	mpicc $(OPT) $(SOURCES) $(KRATOS) -o $(OUTPUT)
quick:
	mpicc $(OPT) $(SOURCES) $(KRATOS) $(QUICK) -o $(OUTPUT)
debug:
	mpicc $(DEBUG) $(KRATOS) $(SOURCES) -o $(OUTPUT)
clean:
	rm *~ $(OUTPUT) $(REPORT)
