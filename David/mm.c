// Had some weird memory errors while counting comp time, couldn't fix :(
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <pthread.h>

#include "rdtsc.h"
#include "MT19937.h"

// Matrix Variables
// AxB = C, R is for communication between MPI ranks
double **A = NULL;
double **B = NULL;
double **C = NULL;
double **R = NULL;
double *A_raw = NULL;
double *B_raw = NULL;
double *C_raw = NULL;
double *R_raw = NULL;
unsigned int matrix_slice;

// MPI Variables
MPI_Request send, recv;
MPI_Status stat;
int taskid, numtasks;
//unsigned long long comm_cycles = 0;
//unsigned long long *comp_cycles;
int nextrank;
int flag;

// Pthread Variables
int num_threads; 
pthread_t *threads;

// Debugging on Kratos, Actual job goes on Q
#ifdef KRATOS
unsigned int matrix_size = 1024;
unsigned int total_threads = 8;
double clock_rate = 2666700000.0;
#else
unsigned int matrix_size = 1024;
unsigned int total_threads = 64;
double clock_rate = 1600000000.0;
#endif

// Random number variables
unsigned long rng_init_seeds[6] = {0x0, 0x123, 0x234, 0x345, 0x456, 0x789};
unsigned long rng_init_length = 6;

void * pthread_multiply(void * args) {
	int current_thread = *(int *)args;
	int totalMults = 0;
	
	// One iteration of mulitplication and communication per task
	for(totalMults = 0; totalMults < numtasks; totalMults++) {
		// Only in 1 thread, post a send for matrix B and a recv for matrix R
		// Communication does not need to happen during the last multiply
		if(current_thread == 0 && totalMults+1 < numtasks) {
			//unsigned long long comm_tmp = rdtsc();
			MPI_Isend(&B[0][0], matrix_size * matrix_slice, MPI_DOUBLE, nextrank, 0, MPI_COMM_WORLD, &send);
			MPI_Irecv(&R[0][0], matrix_size * matrix_slice, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recv);
			//comm_cycles += rdtsc() - comm_tmp;
		}

		// Do the matrix multiply
		//unsigned long long comp_tmp = rdtsc();
		int i_start = current_thread * (matrix_slice / num_threads);
		int i_end = i_start + (matrix_slice / num_threads);
		int i, j, k;
		for(i = i_start; i < i_end; i++)
			for(j = 0; j < matrix_slice; j++)
				for(k = 0; k < matrix_size; k++)
					C[i][j] += A[i][k] * B[k][j];
		//comp_cycles[current_thread] += (rdtsc() - comp_tmp);

		// Wait to terminate until the send and recv have finished
		// Waiting not needed for last multiply
		// comm_cycles only updated for 1 pthread
		if(totalMults+1 < numtasks) {
			//if(current_thread == 0) {
				//unsigned long long comm_tmp = rdtsc();
				
			//	flag = 0;
			//	while(!flag)
			//		MPI_Test(&send, &flag, &stat);
			//	flag = 0;
			//	while(!flag)
			//		MPI_Test(&recv, &flag, &stat);

			//	comm_cycles += rdtsc() - comm_tmp;
			//} else {
				flag = 0;
				while(!flag)
					MPI_Test(&send, &flag, &stat);
				flag = 0;
				while(!flag)
					MPI_Test(&recv, &flag, &stat);
			//}
		}
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	// Initialize MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	nextrank = taskid;
	if(taskid == numtasks)
		nextrank = 0;

	// To compute execution time
	unsigned long long cycle_start = rdtsc();

	// Initialize Random Variable
	rng_init_seeds[0] = taskid;
	init_by_array(rng_init_seeds, rng_init_length);

	// Initialize matrices, memory should be contiguous
	// Inconsistent to show which matrices are wide vs. tall
	matrix_slice = matrix_size / numtasks;
	A_raw = (double *)malloc(matrix_slice * matrix_size * sizeof(double));
	B_raw = (double *)malloc(matrix_size * matrix_slice * sizeof(double));
	C_raw = (double *)malloc(matrix_slice * matrix_slice * sizeof(double));
	R_raw = (double *)malloc(matrix_size * matrix_slice * sizeof(double));

	A = (double **)malloc(matrix_slice * sizeof(double *));
	B = (double **)malloc(matrix_size * sizeof(double *));
	C = (double **)malloc(matrix_slice * sizeof(double *));
	R = (double **)malloc(matrix_size * sizeof(double *));

	// establish columns and rows
	int i;
	for(i = 0; i < matrix_slice; i++)
		A[i] = &A_raw[matrix_size * i];
	for(i = 0; i < matrix_size; i++)
		B[i] = &B_raw[matrix_slice * i];
	for(i = 0; i < matrix_slice; i++)
		C[i] = &C_raw[matrix_slice * i];
	for(i = 0; i < matrix_size; i++)
		R[i] = &R_raw[matrix_slice * i];

	// Fill matrices with random data
	int j;
	for(i = 0; i < matrix_slice; i++)
		for(j = 0; j < matrix_size; j++)
			A[i][j] = genrand_res53();

	for(i = 0; i < matrix_size; i++)
		for(j = 0; j < matrix_slice; j++)
			B[i][j] = genrand_res53();

	// Initialize comp_time
	//comp_cycles = (unsigned long long *)calloc(num_threads, sizeof(unsigned long long));

	// Initialize pthread variables
	num_threads = total_threads / numtasks;
	threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
	int current_thread[num_threads];
	for(i = 0; i < num_threads; i++)
		current_thread[i] = i;

	// Initialize pthreads
	for(i = 0; i < num_threads; i++)
		if(pthread_create(&threads[i], NULL, &pthread_multiply, (void *)&current_thread[i]))
			printf("Could not create thread %i!\n", i);

	// Delete/Join pthreads
	for(i = 0; i < num_threads; i++)
		if(pthread_join(threads[i], NULL))
			printf("Could not join threads!\n");
	
	// Aggregate comp time, which was split between multiple threads
	//for(i = 1; i< num_threads; i++)
		//comp_cycles[0] += comp_cycles[i];

	// Convert all datapoints from clock cycles to seconds
	unsigned long long total_cycles = rdtsc() - cycle_start;
	double runtime_seconds = (double)total_cycles / clock_rate;
	//double comm_seconds = (double)comm_cycles / clock_rate;
	//double comp_seconds = (double)comp_cycles[0] / clock_rate;

	// Create entry points for aggregated data
	//double comp_max, comp_avg, comp_min, comp_sum;
	//double comm_max, comm_avg, comm_min, comm_sum;
	double run_max, run_avg, run_min, run_sum;

	// MPI Reduce All of the Statistics
	MPI_Reduce(&runtime_seconds, &run_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce(&runtime_seconds, &run_min, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Reduce(&runtime_seconds, &run_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	run_avg = run_sum / numtasks;
	
	//MPI_Reduce(&comm_seconds, &comm_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	//MPI_Reduce(&comm_seconds, &comm_min, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	//MPI_Reduce(&comm_seconds, &comm_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	//comm_avg = comm_sum / numtasks;

	//MPI_Reduce(&comp_seconds, &comp_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	//MPI_Reduce(&comp_seconds, &comp_min, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	//MPI_Reduce(&comp_seconds, &comp_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	//comp_avg = comp_sum / numtasks;

	// Runtime: 	Max, Avg, Min
	// Comm Time:	--
	// Comp Time:	--
	if(taskid == 0)
		printf("%g %g %g\n", run_max, run_avg, run_min);

	// Free all memory
	free(A_raw);
	free(B_raw);
	free(C_raw);
	free(R_raw);
	free(A);
	free(B);
	free(C);
	free(R);
	//free(comp_cycles);
	free(threads);

	MPI_Finalize();
	return 0;
}
