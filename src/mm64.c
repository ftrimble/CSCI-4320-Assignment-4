/*******************************************************************************/
/*******************************************************************************/
/***** Forest Trimble                    ***************************************/
/***** trimbf@rpi.edu                    ***************************************/
/***** Assignment 3:                     ***************************************/
/*****   MPI Matrix Multiply and         ***************************************/
/*****   Performance Analysis            ***************************************/
/***** Due: March 19, 2013               ***************************************/
/*******************************************************************************/
/*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#include "rdtsc.h"
#include "MT19937.h"

double **A = NULL;
double **B = NULL;
double **B_recv = NULL;
double **C = NULL;

#ifdef QUICK
unsigned int matrix_size=1024;
#else 
unsigned int matrix_size=8192;
#endif 

unsigned int total_threads = 64;

unsigned long rng_init_seeds[6]={0x0, 0x123, 0x234, 0x345, 0x456, 0x789};
unsigned long rng_init_length=6;

#ifdef KRATOS
double clock_rate=2666700000.0; 
#else /* Using Blue Gene/Q */
double clock_rate=1600000000.0; 
#endif

/* This function allocates a contiguous block of memory *
 * so that MPI_Isend/Irecv operations are much easier.  */
double ** alloc2dcontiguous(int rows, int cols) {
  int i;
  double *data = (double *)calloc(rows*cols, sizeof(double)),
    **array = (double **)calloc(rows, sizeof(double));

  for ( i = 0; i < rows; ++i ) array[i] = &data[cols*i];

  return array;
}

/* Receives data about all the times at node 0.   *
 * This is the node that will perform output.     *
 * note that since we do not need every processor *
 * to have this data, I performed a reduce rather *
 * than an allReduce, although the assignment     *
 * indicated otherwise.                           */
void performReduces(double *send, double *data, int numtasks) {
  MPI_Reduce(send,&data[0],1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
  MPI_Reduce(send,&data[1],1,MPI_DOUBLE,MPI_MIN,0,MPI_COMM_WORLD);
  MPI_Reduce(send,&data[2],1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);

  // divides average by numtasks
  data[0] /= numtasks;
}

/* Opens a file with the proper name, and sets the pointer properly. */
void openFiles(FILE **file, int i, int j) {
  char fileName[256], typeName[256], dataName[256];
  
  switch (i) {
  case 0:          sprintf(typeName,"exec");                            break;
  case 1:          sprintf(typeName,"comm");                            break;
  case 2:          sprintf(typeName,"mm");                              break;
  }
  switch (j) {
  case 0:          sprintf(dataName,"Avg");                             break;
  case 1:          sprintf(dataName,i == 1 ? "Max" : "Min");            break;
  case 2:          sprintf(dataName,i == 1 ? "Min" : "Max");            break;
  }
  sprintf(fileName,"data/%s%s.dat",typeName,dataName);
      
  *file = fopen(fileName,"a");
}

void * pthread_multiply(void * args) {
  // note funky math due to B in column major order
  for ( i = 0; i < matrix_size/numtasks; ++i ) {
    for ( j = 0; j < matrix_size/numtasks; ++j ) {
for ( k = 0; k < matrix_size; ++k ) {
  int sourceCol = taskid - block;
  if ( sourceCol < 0 ) sourceCol += numtasks;
	  C[i][j + sourceCol*matrix_size/numtasks] += A[i][k]*B[j][k];
	}
      }
    }
}

int main(int argc, char *argv[]) {
  int i, j, k,
    block = 0,                                           
    taskid, numtasks;
  double **tmp,
    execTime = 0, execStart,
    mmTime = 0, mmStart, 
    commTime = 0, commStart,
    data[3][3];
  MPI_Request send, recv;
  MPI_Status stat;
 
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  execStart = rdtsc();
  
  // seed MT19937
  rng_init_seeds[0] = taskid;
  init_by_array(rng_init_seeds,rng_init_length);

  // Init rows of A and C, and columns of B, B_recv:
  A = alloc2dcontiguous(matrix_size/numtasks,matrix_size);
  B = alloc2dcontiguous(matrix_size/numtasks,matrix_size);
  B_recv = alloc2dcontiguous(matrix_size/numtasks,matrix_size);
  C = alloc2dcontiguous(matrix_size/numtasks,matrix_size);

  /* Note that we have allocated a chunk of B that must implicitly be *
   * in column major order. That is, our math will look a little      *
   * funky, becuase access will be reversed in A,C vs. B.             */

  // fills A, B with random numbers.
  for ( i = 0; i < matrix_size/numtasks; ++i ) {
    for ( j = 0; j < matrix_size; ++j ) {
      A[i][j] = genrand_res53();
      B[i][j] = genrand_res53();
    }
  }

	// Generate pthreads
	int numPthreads = total_threads / numtasks;
	pthread_t threads[numPthreads];

  // now perform multiply and sends
  while ( block < numtasks ) { 
    /* sends out B buffer and receives to a second *
     * buffer. note that the final iteration just  *
     * does math; it does not send anything.       */
    if ( block < numtasks - 1 ) {
      int sourceId = ( taskid == 0 ) ? numtasks-1 : taskid-1,
	sendId = ( taskid == numtasks - 1 ) ? 0 : taskid + 1;

      commStart = rdtsc();
      MPI_Isend(&B[0][0],matrix_size*matrix_size/numtasks,MPI_DOUBLE,
		sendId,taskid,MPI_COMM_WORLD,&send);
      MPI_Irecv(&B_recv[0][0],matrix_size*matrix_size/numtasks,MPI_DOUBLE,
		sourceId,MPI_ANY_TAG,MPI_COMM_WORLD,&recv);
      commTime += rdtsc() - commStart;
    }

		int z;
    mmStart = rdtsc();
		for(z = 0; z < total_threads; z++)
			if(pthread_create(&threads[z], NULL, &pthread_multiply, NULL))
				printf("Thread creation failed\n");
		for(z = 0; z < total_threads; z++)
			if(pthread_join(thr, NULL))
				printf("Thread joining failed\n");

    mmTime += rdtsc() - mmStart;

    commStart = rdtsc();
    /* wait for isend/irecv to complete, as we can't *
     * do the next set until these have finished     */
    MPI_Wait(&send,&stat);       MPI_Wait(&recv,&stat);       
    commTime += rdtsc() - commStart;
    
    /* swaps pointer locations so that we can use B *
     * for math and receive data on B_recv without  *
     * having them interfere with each other.       */
    tmp = B;         B = B_recv;         B_recv = tmp;
    
    ++block;
  }
  
  // calculates the times in seconds that we used.
  execTime = (rdtsc() - execStart)/clock_rate;
  commTime /= clock_rate;
  mmTime /= clock_rate;

  // reduces our times to the values over all processors.
  performReduces(&execTime, data[0], numtasks);
  performReduces(&commTime, data[1], numtasks);
  performReduces(&mmTime, data[2], numtasks);
  
  // bandwidth = bytes / time
  for ( i = 0; i < 3; ++i ) 
    data[1][i] = 
      matrix_size*matrix_size*(numtasks-1)*sizeof(double)/numtasks/data[1][i];

  // node 0 performs output
  if ( taskid == 0 ) {
    // only outputs the full chunk in the 
    // smaller matrix size.
#ifdef QUICK
    for ( i = 0; i < matrix_size/numtasks; ++i ) {
      for ( j = 0; j < matrix_size; ++j )
	printf("%e ",C[i][j]);
      printf("\n");
    }
#elif defined KRATOS
    FILE **dataFiles;
    dataFiles = (FILE **)calloc(9,sizeof(FILE *));
#endif

    // doesn't print data to a file if quick is defined
#ifndef QUICK
    for ( i = 0; i < 9; ++i ) {
#ifdef KRATOS
      /* This is something that did not work on Blue Gene -   *
       * it seems that creating files is not acceptable here. *
       * To compensate for this, I have created a shell       *
       * called getdata.sh, which takes this output and       *
       * converts it to the same files that we would normally *
       * generate.                                            */
      openFiles(&dataFiles[i],i/3,i%3);
      fprintf(dataFiles[i],"%d %e\n",numtasks,data[i/3][i%3]);
      fclose(dataFiles[i]);
#else 
      /* We simply call a printf on Blue Gene */
      printf("%d %e\n\n",numtasks,data[i/3][i%3]); 
#endif
    }
#endif 

#if defined KRATOS && !defined QUICK
    free(dataFiles);
#endif
  }

  /* frees up the memory allocated for our arrays. *
   * recall that the array was initiated in one    *
   * contiguous chunk, so one call to free should  *
   * deallocate the whole underlying structure.    */
  free(&A[0][0]);                            free(A);
  free(&B[0][0]);                            free(B);
  free(&B_recv[0][0]);                  free(B_recv);
  free(&C[0][0]);                            free(C);

  MPI_Finalize();

  return 0;
}
