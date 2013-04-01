/******************************************************************************
 * FILE: mm.c
 * assignment3 - Parallel Matrix Multiply on the IBM Blue Gene/Q
 * Program written by Scott Todd, March 2013 for Parallel Programming and 
 *     Computing (CSCI 4320)
 *
 * Compile using the provided makefile or...
 *      Kratos:         mpicc -o mm -Wall mm.c -lm
 *
 *      Blue Gene/Q:    module load xl
 *                      mpixlc -o mm -O3 mm.c -lm
 * Run using..
 *      Kratos:         mpirun -np [NUM_CORES] ./mm
 *
 *      Blue Gene/Q:    srun -o [OUTPUT_FILE_NAME] --partition=small --time=10 --nodes=[NODES] --ntasks=[NTASKS] --overcommit --runjob-opts="--mapping TEDCBA" ./mm
 *                      OR... run batch_creator.py to generate batch files
 *                      and run "sbatch [BATCH_FILE_NAME]" or master_batch.sh,
 *                      which submits a job for each batch file created
 *
 * Parse output using output_parser.py (I then imported into excel)
 *
 * This parallel C program performs memory contrained matrix multiplcation
 * using a variable number of MPI ranks.
 *
 * Each MPI rank initializes one row section of matrix A and one column section
 * of matrix B. Each rank then multiplies the blocks of A and B to completely
 * calculate part of matrix C, then passes its column section of B to the next
 * rank in the cycle. When all ranks finish computing their rows of C, the
 * program terminates and rank 0 outputs its section of C.
 *
 * Notes:
 *   - total execution time is measured by calling rdtsc in rank 0 at the start
 *     and end of program execution
 *   - rather than use a single additional buffer, I used 13 buffers,
 *     1 working buffer + 6 isend buffers + 6 irecv buffers, to handle
 *     the (probably?) rare case that a given rank lags behind the others
 *
 *****************************************************************************/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "rdtsc.c"
#include "mt19937.c"

#define UNUSED_BUFFER  -1   // used in the buffer_type array
#define RECV_BUFFER    0    // used in the buffer_type array
#define SEND_BUFFER    1    // used in the buffer_type array
#define WORKING_BUFFER 2    // currently or about to be used for multiplcation

unsigned int MATRIX_SIZE = 8192;

double CLOCK_RATE = 2666700000.0; // Kratos
// double CLOCK_RATE = 1600000000.0; // Blue Gene/Q

// Multiply matrix A by matrix B and store the results in part of matrix C
// -------- Arguments --------
// A is a [a_rows x size] matrix
// B is a [size x b_columns] matrix
// The resultant part of C, originating at (c_start_x, c_start_y), is a
//   size x size matrix
// ---------------------------
// --- Example application ---
// A is a 128 x 1024 block of a 1024 x 1024 matrix
// B is a 1024 x 128 block of another 1024 x 1024 matrix (the third such block)
// C is a 128 x 1024 matrix, and the result of A x B should be stored starting 
//      at index (0, 256), since the first block is in columns 0-127 and the
//      second block is in columns 128-255
// ---------------------------
// Matrix multiplication reference:
//      **   ***   **       #*   ###    #*
//      ** X *** = ** where #* X *** -> **
//      **                  #*
// ---------------------------
void matrix_multiply( double **A, double **B, double **C, 
                      int size, int a_rows, int b_columns, 
                      int c_start_x, int c_start_y ) {
    int i, j, k;

    for( i = 0; i < a_rows; i++ ) {
        for( j = 0; j < size; j++ ) {
            for( k = 0; k < b_columns; k++ ) {
                C[ i + c_start_x ][ k + c_start_y ] += A[ i ][ j ] * B[ j ][ k ];
            }
        }
    }                          
}

void print_matrix( double **A, int rows, int columns ) {
    int i, j;
    
    for( i = 0; i < rows; i++ ) {
        for( j = 0; j < columns; j++) {
            printf( "%f ", A[i][j] );
        }
        printf( "\n" );
    }
    printf( "\n" );
}

// allocates (using calloc) a contiguous 2D array of doubles
double **allocate_contiguous_2D_double(int rows, int columns) {
    int i;
    
    // first allocate one large block of doubles
    double *contiguous_data = (double *)calloc( rows*columns, sizeof(double) );
    // then create a pointer to a 2D array
    double **array = (double **)calloc( rows, sizeof(double*) );
    
    // map sections of the contiguous block to rows in the 2D array
    for ( i = 0; i < rows; i++ ) {
        array[i] = &( contiguous_data[columns * i] );
    }
    
    // return the pointer to the 2D array
    return array;
}

int main( int argc, char **argv ) {
    /* -------------------------------------------------- */
    // Declare variables
    int             numranks,           // number of tasks
                    myrank,             // rank for this task
                    NUM_BUFFERS = 13,   // number of buffers
                    MAX_RECEIVES = 6,   // limits posted_receives    
                    posted_receives = 0,// limit to < MAX_RECEIVES
                    posted_sends = 0,   // 
                    destination,        // Isend destination rank
                    message_tag,        // type of message
                    current_block,      // 0 - (numranks - 1) blocks
                    working_buffer_index,   // buffer index being used for mult.
                    blocks_multiplied = 0,  // number of matrix blocks processed
                    flag = 0,           // MPI_Test true/false
                    i, j;               // misc (loops)
                    
    
    // buffers: 1 working, 6 receive, 6 send/unused
    // assuming all ranks work at around the same rate, 
    // there shouldn't be a large backup of sends/receives
    // planning for 6 sends/receives at a time keeps memory usage low
    double**    buffers[NUM_BUFFERS]; 
    MPI_Request requests[NUM_BUFFERS];
    int         buffer_type[NUM_BUFFERS];
                    
    MPI_Status          status;                     // status for MPI_Test
    unsigned long long  total_isend_cycles = 0,     // MPI_Isend processing
                        total_irecv_cycles = 0,     // MPI_Irecv processing
                        total_multi_cycles = 0,     // matrix multiplcation
                        start_cycles,               // cpu cycles
                        end_cycles,                 // cpu cycles
                        start_cycles2,              // cpu cycles
                        end_cycles2,                // cpu cycles
                        total_time_start_cycles,    // cpu cycles
                        total_time_end_cycles;      // cpu cycles
                        
    double              total_isend_time = 0,       // seconds
                        total_irecv_time = 0,       // seconds
                        total_multi_time = 0;       // seconds
                        
    double              all_min_isend_time,         // seconds
                        all_max_isend_time,         // seconds
                        all_avg_isend_time,         // seconds
                        all_min_irecv_time,         // seconds
                        all_max_irecv_time,         // seconds
                        all_avg_irecv_time,         // seconds
                        all_min_multi_time,         // seconds
                        all_max_multi_time,         // seconds
                        all_avg_multi_time;         // seconds
                        
    double              all_min_isend_bandwidth,    // bytes per second
                        all_max_isend_bandwidth,    // bytes per second
                        all_avg_isend_bandwidth,    // bytes per second
                        all_min_irecv_bandwidth,    // bytes per second
                        all_max_irecv_bandwidth,    // bytes per second
                        all_avg_irecv_bandwidth;    // bytes per second
                        
    double              total_time;                 // seconds
                        
    int                 total_bytes_sent;
    
    double **A = NULL;
    double **C = NULL;
    unsigned long rng_init_seeds[6] = {0x0, 0x123, 0x234, 0x345, 0x456, 0x789};
    unsigned long RNG_INIT_LENGTH = 6;
    
    /* -------------------------------------------------- */
    
    
    /* -------------------------------------------------- */
    // MPI Startup/Initialization
    
    MPI_Init(&argc, &argv);
    
    total_time_start_cycles = rdtsc();
    
    // get stats for this rank
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numranks);
    
    // seed the RNG using myrank
    rng_init_seeds[0] = myrank;
    init_by_array(rng_init_seeds, RNG_INIT_LENGTH);
    
    // initialize buffer_type and allocate memory for buffers
    for ( i = 0; i < NUM_BUFFERS; i++ ) {
        buffer_type[i] = UNUSED_BUFFER;
        buffers[i] = 
           allocate_contiguous_2D_double( MATRIX_SIZE, MATRIX_SIZE / numranks );
    }
    /* -------------------------------------------------- */
    
    
    /* -------------------------------------------------- */
    // Allocate and initialize matrices
    
    buffer_type[0] = WORKING_BUFFER;
    working_buffer_index = 0;
    
    A = allocate_contiguous_2D_double( MATRIX_SIZE / numranks, MATRIX_SIZE );
    // blocks of B are stored in buffers
    C = allocate_contiguous_2D_double( MATRIX_SIZE / numranks, MATRIX_SIZE );
    
    current_block = myrank;
    
    for( i = 0; i < MATRIX_SIZE / numranks; i++ ) {
        for( j = 0; j < MATRIX_SIZE; j++ ) {
            A[i][j] = genrand_res53();
            buffers[working_buffer_index][j][i] = genrand_res53();
            C[i][j] = 0;
        }
    }
    /* -------------------------------------------------- */
    
    
    /* -------------------------------------------------- */
    // Post MAX_RECEIVES initial receive requests
    start_cycles = rdtsc(); // posting irecv
    
    for ( i = 1; i < MAX_RECEIVES; i++ ) {
        buffer_type[i] = RECV_BUFFER;
                 
        MPI_Irecv( &(buffers[i][0][0]),
                   MATRIX_SIZE * MATRIX_SIZE / numranks, // rows * cols elements
                   MPI_DOUBLE,
                   MPI_ANY_SOURCE,
                   MPI_ANY_TAG,
                   MPI_COMM_WORLD,
                   &requests[i]
                 );
                 
        posted_receives++;
    }
    
    end_cycles = rdtsc();
    total_irecv_cycles += end_cycles - start_cycles;
    /* -------------------------------------------------- */
    
    
    /* -------------------------------------------------- */
    // Setup complete, start multiplying and message passing!
    
    while ( blocks_multiplied < numranks ) {
        
        /* ------------------------------------------------------------- */
        // Matrix multiplcation section

        start_cycles = rdtsc();
        
        matrix_multiply( A, buffers[working_buffer_index], C,
                         MATRIX_SIZE,            // a_cols, b_rows, c_rows, c_cols
                         MATRIX_SIZE / numranks, // a_rows
                         MATRIX_SIZE / numranks, // b_columns
                         0,                      // c_start_x,
                         current_block * (MATRIX_SIZE / numranks) // c_start_y
                       );
                       
        blocks_multiplied++;
        
        end_cycles = rdtsc();
        total_multi_cycles += end_cycles - start_cycles;
        /* ------------------------------------------------------------- */
        
        
        /* ------------------------------------------------------------- */
        // Message passing section
        
        // send the working buffer to the next rank in the cycle
        
        
        destination = ( myrank + 1 ) % numranks;
        message_tag = current_block;
        
        if ( message_tag != destination ) {
            start_cycles = rdtsc(); // posting isend
            
            buffer_type[working_buffer_index] = SEND_BUFFER;
            
            MPI_Isend( &(buffers[working_buffer_index][0][0]), 
                       MATRIX_SIZE * MATRIX_SIZE / numranks, // rows * cols elements
                       MPI_DOUBLE, 
                       destination, 
                       message_tag, 
                       MPI_COMM_WORLD,
                       &requests[working_buffer_index]
                     );
                     
            posted_sends++;
            
            end_cycles = rdtsc();
            total_isend_cycles += end_cycles - start_cycles;
        } else {
            buffer_type[working_buffer_index] = UNUSED_BUFFER;
        }
        
        // loop over buffers/requests until we receive the next block of B
        //      while looping, check for the Isend(s) completing and
        //      post additional Irecvs as buffers become available
        current_block = -1;
        
        do { 
            for ( i = 0; i < NUM_BUFFERS; i++ ) {
                switch ( buffer_type[i] ) {
                    
                    /* -------------------------------------------------- */
                    case SEND_BUFFER:
                        // this is a send request and buffer
                        // check if the message has been fully sent
                        
                        start_cycles2 = rdtsc(); // checking isend
                    
                        MPI_Test( &requests[i], &flag, &status );
                        
                        if ( flag ) { // message has been fully sent
                            // convert this buffer to a 
                            // receive buffer or an unused buffer,
                            // depending on the number of posted receives
                            if ( posted_receives < MAX_RECEIVES ) {   
                                start_cycles = rdtsc(); // posting irecv
                                
                                buffer_type[i] = RECV_BUFFER;
                                
                                MPI_Irecv( &(buffers[i][0][0]),
                                           MATRIX_SIZE * MATRIX_SIZE / numranks,
                                           MPI_DOUBLE,
                                           MPI_ANY_SOURCE,
                                           MPI_ANY_TAG,
                                           MPI_COMM_WORLD,
                                           &requests[i]
                                         );
                                
                                posted_receives++;
                                
                                end_cycles = rdtsc();
                                total_irecv_cycles += end_cycles - start_cycles;
                            } else {
                                buffer_type[i] = UNUSED_BUFFER;
                            }
                            posted_sends--;
                        }
                            
                        end_cycles2 = rdtsc();
                        total_isend_cycles += end_cycles2 - start_cycles2;
                    
                        break;
                    /* -------------------------------------------------- */
                    
                    /* -------------------------------------------------- */
                    case RECV_BUFFER:
                        // this is a receive request and buffer
                        // check if we've received a block of the B matrix
                        
                        start_cycles = rdtsc(); // checking irecv
                        
                        // only test for new matrices if we've not yet 
                        // received one during this loop
                        if ( current_block == -1 ) {
                            MPI_Test( &requests[i], &flag, &status );
                    
                            if ( flag ) { // message received
                                posted_receives--;
                                
                                // get the new working buffer
                                buffer_type[i] = WORKING_BUFFER;
                                working_buffer_index = i;
                                current_block = status.MPI_TAG;
                            }
                        }
                        
                        end_cycles = rdtsc();
                        total_irecv_cycles += end_cycles - start_cycles;
                        
                        break;
                    /* -------------------------------------------------- */
                    
                    /* -------------------------------------------------- */
                    case WORKING_BUFFER:
                        break;
                    /* -------------------------------------------------- */
                    
                    /* -------------------------------------------------- */
                    case UNUSED_BUFFER:
                        if ( posted_receives < MAX_RECEIVES ) {
                            start_cycles = rdtsc(); // posting irecv
                            
                            buffer_type[i] = RECV_BUFFER;
                            
                            MPI_Irecv( &(buffers[i][0][0]),
                                       MATRIX_SIZE * MATRIX_SIZE / numranks,
                                       MPI_DOUBLE,
                                       MPI_ANY_SOURCE,
                                       MPI_ANY_TAG,
                                       MPI_COMM_WORLD,
                                       &requests[i]
                                     );
                                
                            posted_receives++;
                            
                            end_cycles = rdtsc();
                            total_irecv_cycles += end_cycles - start_cycles;
                        }
                        
                        break;
                    /* -------------------------------------------------- */
                        
                    /* -------------------------------------------------- */
                    default:
                        // ? buffer type isn't one of thoes enumerated..?
                        break;
                    /* -------------------------------------------------- */
                }
            }
            
            // do-while with blocks_multiplied < numranks ensures that
            // receives are completed
        } while ( current_block == -1 && blocks_multiplied < numranks );
        // -------------------------------------------------------------
        
    }
    
    // wait for posted sends to complete
    // might not be needed, but good to clean up?
    while ( posted_sends > 0 ) {
        for ( i = 0; i < NUM_BUFFERS; i++ ) {
            switch ( buffer_type[i] ) {
                
                /* -------------------------------------------------- */
                case SEND_BUFFER:            
                    // this is a send request and buffer
                    // check if the message has been fully sent
                    
                    start_cycles = rdtsc(); // checking isend
                    
                    MPI_Test( &requests[i], &flag, &status );
                    
                    if ( flag ) { // message has been fully sent
                        buffer_type[i] = UNUSED_BUFFER;
                        posted_sends--;
                    }
                    
                    end_cycles = rdtsc();
                    total_isend_cycles += end_cycles - start_cycles;
                
                    break;
                /* -------------------------------------------------- */
                
                /* -------------------------------------------------- */
                default:
                    break;
                /* -------------------------------------------------- */
            }
        }
    }
    
    // wait for all processes to terminate and collect metrics
    MPI_Barrier( MPI_COMM_WORLD ); 
    
    total_time_end_cycles = rdtsc();
    
    total_isend_time = total_isend_cycles / CLOCK_RATE;
    total_irecv_time = total_irecv_cycles / CLOCK_RATE;
    total_multi_time = total_multi_cycles / CLOCK_RATE;
    
    total_time = (total_time_end_cycles - total_time_start_cycles) / CLOCK_RATE;
    
    // Perform MPI_Allreduces on the metrics collected
    
    MPI_Allreduce( &total_isend_time,
                   &all_min_isend_time,
                   1,
                   MPI_DOUBLE,
                   MPI_MIN,
                   MPI_COMM_WORLD
                 );
                 
    MPI_Allreduce( &total_isend_time,
                   &all_max_isend_time,
                   1,
                   MPI_DOUBLE,
                   MPI_MAX,
                   MPI_COMM_WORLD
                 );
                 
    MPI_Allreduce( &total_isend_time,
                   &all_avg_isend_time,
                   1,
                   MPI_DOUBLE,
                   MPI_SUM,
                   MPI_COMM_WORLD
                 );
    all_avg_isend_time /= numranks;
    
    MPI_Allreduce( &total_irecv_time,
                   &all_min_irecv_time,
                   1,
                   MPI_DOUBLE,
                   MPI_MIN,
                   MPI_COMM_WORLD
                 );
                 
    MPI_Allreduce( &total_irecv_time,
                   &all_max_irecv_time,
                   1,
                   MPI_DOUBLE,
                   MPI_MAX,
                   MPI_COMM_WORLD
                 );
                 
    MPI_Allreduce( &total_irecv_time,
                   &all_avg_irecv_time,
                   1,
                   MPI_DOUBLE,
                   MPI_SUM,
                   MPI_COMM_WORLD
                 );
    all_avg_irecv_time /= numranks;
    
    MPI_Allreduce( &total_multi_time,
                   &all_min_multi_time,
                   1,
                   MPI_DOUBLE,
                   MPI_MIN,
                   MPI_COMM_WORLD
                 );
                 
    MPI_Allreduce( &total_multi_time,
                   &all_max_multi_time,
                   1,
                   MPI_DOUBLE,
                   MPI_MAX,
                   MPI_COMM_WORLD
                 );
                 
    MPI_Allreduce( &total_multi_time,
                   &all_avg_multi_time,
                   1,
                   MPI_DOUBLE,
                   MPI_SUM,
                   MPI_COMM_WORLD
                 );
    all_avg_multi_time /= numranks;
    
    if ( myrank == 0 ) {
        printf( "Rank %d/%d: \n", myrank, numranks );
        
        total_bytes_sent = MATRIX_SIZE * MATRIX_SIZE / numranks * (numranks-1);
        all_min_isend_bandwidth = total_bytes_sent / all_max_isend_time;
        all_max_isend_bandwidth = total_bytes_sent / all_min_isend_time;
        all_avg_isend_bandwidth = total_bytes_sent / all_avg_isend_time;
        
        all_min_irecv_bandwidth = total_bytes_sent / all_max_irecv_time;
        all_max_irecv_bandwidth = total_bytes_sent / all_min_irecv_time;
        all_avg_irecv_bandwidth = total_bytes_sent / all_avg_irecv_time;
        
        printf( "Total execution time (s), %.7f\n", total_time );
        printf( "                min,       max,       avg\n" );
        printf( "Isend time (s), %.7f, %.7f, %.7f\n", 
            all_min_isend_time, all_max_isend_time, all_avg_isend_time );
        printf( "Irecv time (s), %.7f, %.7f, %.7f\n", 
            all_min_irecv_time, all_max_irecv_time, all_avg_irecv_time );
        printf( "Isend bandwidth (bytes/sec), %.0f, %.0f, %.0f\n", 
                all_min_isend_bandwidth, 
                all_max_isend_bandwidth, 
                all_avg_isend_bandwidth 
              );
        printf( "Irecv bandwidth (bytes/sec), %.0f, %.0f, %.0f\n", 
                all_min_irecv_bandwidth, 
                all_max_irecv_bandwidth, 
                all_avg_irecv_bandwidth
              );
        printf( "Multi time (s), %.7f, %.7f, %.7f\n", 
                all_min_multi_time, 
                all_max_multi_time, 
                all_avg_multi_time
              );
              
        print_matrix( C, MATRIX_SIZE / numranks, MATRIX_SIZE );
    }
    
    for ( i = 0; i < NUM_BUFFERS; i++ ) {
        free( buffers[i] );
    }
    free( A );
    free( C );
    
    MPI_Finalize();
    
    return EXIT_SUCCESS;
}
