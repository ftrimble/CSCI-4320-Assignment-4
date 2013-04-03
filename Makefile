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
OUTPUT=-o mm_threaded
DEBUG=-Wall -DDEBUG_MODE
KRATOS=-DKRATOS
BLUE=-O3 -DBLUE
QUICK=-DQUICK
OPT=-O7
REPORT=findings.aux \
       findings.log \
       findings.pdf

blue:
	mpicc $(SOURCES) $(BLUE) $(OUTPUT)
kratos:
	mpicc $(OPT) $(SOURCES) $(KRATOS) $(OUTPUT)
quick:
	mpicc $(OPT) $(SOURCES) $(KRATOS) $(QUICK) $(OUTPUT)
debug:
	mpicc $(DEBUG) $(KRATOS) $(SOURCES) $(OUTPUT)
clean:
	rm *~ $(OUTPUT) $(REPORT)
