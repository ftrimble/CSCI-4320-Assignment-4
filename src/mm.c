/*******************************************************************************/
/*******************************************************************************/
/***** Forest Trimble,               *******************************************/
/***** Scott Todd,		     *******************************************/
/***** David Vorick                  *******************************************/
/***** {trimbf,voricd,todds}@rpi.edu *******************************************/
/***** Assignment 4:                 *******************************************/
/*****   MPI Matrix Multiply and     *******************************************/
/*****   Performance Analysis        *******************************************/
/*****         with multithreading   *******************************************/
/***** Due: April 9, 2013            *******************************************/
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

int taskid, numtasks, // MPI info
	numThreads,					// Pthread count
  block = 0;          // How many chunks we have done

#ifdef KRATOS
unsigned int matrix_size = 512;
unsigned int total_threads = 8;
#else
unsigned int matrix_size = 1024;
unsigned int total_threads = 64;
#endif

// random number stuff
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

void * pthread_multiply(void * args) {
  int i, j, k, 
    sourceCol,
		start, end;

  int current_thread = (int)args;
  start = taskid * (matrix_size/numtasks/numThreads) + current_thread * (matrix_size/numtasks/numThreads);
  end = taskid * (matrix_size/numtasks/numThreads) + (matrix_size/numtasks/numThreads) current_thread * (matrix_size/numtasks/numThreads);

printf("%i: starting multiply: %i-%i \n", taskid, start, end);

  // note funky math due to B in column major order
  for ( i = start; i < end; ++i ) {
    for ( j = 0; j < matrix_size/numtasks; ++j ) {
      for ( k = 0; k < matrix_size; ++k ) {
	sourceCol = taskid - block;
	if ( sourceCol < 0 ) sourceCol += numtasks;
	C[i][j + sourceCol*matrix_size/numtasks] += A[i][k]*B[j][k];
      }
    }
  }
printf("%i: ending multiply: %i-%i \n", taskid, start, end);
}

int main(int argc, char *argv[]) {
  printf("Main Started!\n");

  int i, j;               // Indeces
  double **tmp,           // Swaps B and B_temp
    execTime, execMax;    // For reporting data
  MPI_Request send, recv; // Need these to check...
  MPI_Status stat;        // ...status of send/recv
  pthread_t *threads;
 
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  printf("MPI Initialized\n");

  execTime = rdtsc();
  
  // seed MT19937
  rng_init_seeds[0] = taskid;
  init_by_array(rng_init_seeds,rng_init_length);


  // Init rows of A and C, and columns of B, B_recv:
  A = alloc2dcontiguous(matrix_size/numtasks,matrix_size);
  B = alloc2dcontiguous(matrix_size/numtasks,matrix_size);
  B_recv = alloc2dcontiguous(matrix_size/numtasks,matrix_size);
  C = alloc2dcontiguous(matrix_size/numtasks,matrix_size);

  // allocate memory for our threads
  numThreads = total_threads/numtasks;
  threads = (pthread_t *)calloc(numThreads, sizeof(pthread_t));

  /* Note that we have allocated a chunk of B that must implicitly be *
   * in column major order. That is, our math will look a little      *
   * funky, becuase access will be reversed in A,C vs. B.             */

printf("Initializing Array!\n");

  // fills A, B with random numbers.
  for ( i = 0; i < matrix_size/numtasks; ++i ) {
    for ( j = 0; j < matrix_size; ++j ) {
      A[i][j] = genrand_res53();
      B[i][j] = genrand_res53();
    }
  }

printf("random numbers filled\n");

  // now perform multiply and sends
  while ( block < numtasks ) { 
printf("block: %i\n", block);
    /* sends out B buffer and receives to a second *
     * buffer. note that the final iteration just  *
     * does math; it does not send anything.       */
    if ( block < numtasks - 1 ) {
      int sourceId = ( taskid == 0 ) ? numtasks-1 : taskid-1,
	sendId = ( taskid == numtasks - 1 ) ? 0 : taskid + 1;

      MPI_Isend(&B[0][0],matrix_size*matrix_size/numtasks,MPI_DOUBLE,
		sendId,taskid,MPI_COMM_WORLD,&send);
      MPI_Irecv(&B_recv[0][0],matrix_size*matrix_size/numtasks,MPI_DOUBLE,
		sourceId,MPI_ANY_TAG,MPI_COMM_WORLD,&recv);
    }

    // threads out the matrix multiplication
    for(i = 0; i < numThreads; i++) 
      if( pthread_create(&threads[i], NULL, &pthread_multiply, (void *)i) )
	printf("Thread creation failed\n");

    // joins the threads back together
    for(i = 0; i < numThreads; i++) 
      if(pthread_join(threads[i], NULL))
	printf("Thread joining failed\n");

    /* wait for isend/irecv to complete, as we can't *
     * do the next set until these have finished     */
    MPI_Wait(&send,&stat);       MPI_Wait(&recv,&stat);       
    
    /* swaps pointer locations so that we can use B *
     * for math and receive data on B_recv without  *
     * having them interfere with each other.       */
    tmp = B;         B = B_recv;         B_recv = tmp;
    
    ++block;
  }
  
  // calculates the times in seconds that we used.
  execTime = (rdtsc() - execTime)/clock_rate;

  // reduces our time to the max over all processors.
  MPI_Reduce(&execTime,&execMax,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
  
  // node 0 performs output: 
  if ( taskid == 0 ) printf("%d %e",numThreads,execMax);

  /* frees up the memory allocated for our arrays. *
   * recall that the array was initiated in one    *
   * contiguous chunk, so one call to free should  *
   * deallocate the whole underlying structure.    */
  free(&A[0][0]);                            free(A);
  free(&B[0][0]);                            free(B);
  free(&B_recv[0][0]);                  free(B_recv);
  free(&C[0][0]);                            free(C);
  free(threads);

  MPI_Finalize();

  return 0;
}
