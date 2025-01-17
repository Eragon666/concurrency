/*
 * assign3_1.c
 *
 * Contains code for setting up and finishing the simulation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mpi.h"
#include "file.h"
#include "timer.h"
#include "simulate.h"

typedef double (*func_t)(double x);

/*
 * Simple gauss with mu=0, sigma^1=1
 */
double gauss(double x)
{
    return exp((-1 * x * x) / 2);
}


/*
 * Fills a given array with samples of a given function. This is used to fill
 * the initial arrays with some starting data, to run the simulation on.
 *
 * The first sample is placed at array index `offset'. `range' samples are
 * taken, so your array should be able to store at least offset+range doubles.
 * The function `f' is sampled `range' times between `sample_start' and
 * `sample_end'.
 */
void fill(double *array, int offset, int range, double sample_start,
        double sample_end, func_t f)
{
    int i;
    float dx;

    dx = (sample_end - sample_start) / range;
    for (i = 0; i < range; i++) {
        array[i + offset] = f(sample_start + i * dx);
    }
}

void fillmpi(double *array, int offset, int range, int  double sample_start,
        double sample_end, func_t f)
{
    int i;
    float dx;

    dx = (sample_end - sample_start) / range;
    for (i = 1; i < 

}

int main(int argc, char *argv[])
{
    double *old, *current, *next, *ret;
    int t_max, i_max, rc, rank, i, size;
    double time;

    /* Parse commandline args */
    if (argc < 3) {
        printf("Usage: %s i_max t_max num_threads [initial_data]\n", argv[0]);
        printf(" - i_max: number of discrete amplitude points, should be >2\n");
        printf(" - t_max: number of discrete timesteps, should be >=1\n");
        printf(" - num_threads: number of threads to use for simulation, "
                "should be >=1\n");
        printf(" - initial_data: select what data should be used for the first "
                "two generation.\n");
        printf("   Available options are:\n");
        printf("    * sin: one period of the sinus function at the start.\n");
        printf("    * sinfull: entire data is filled with the sinus.\n");
        printf("    * gauss: a single gauss-function at the start.\n");
        printf("    * file <2 filenames>: allows you to specify a file with on "
                "each line a float for both generations.\n");

        return EXIT_FAILURE;
    }

    i_max = atoi(argv[1]);
    t_max = atoi(argv[2]);

    if (i_max < 3) {
        printf("argument error: i_max should be >2.\n");
        return EXIT_FAILURE;
    }
    if (t_max < 1) {
        printf("argument error: t_max should be >=1.\n");
        return EXIT_FAILURE;
    }

    /* Allocate and initialize buffers. */
    old = malloc(i_max * sizeof(double));
    current = malloc(i_max * sizeof(double));
    next = malloc(i_max * sizeof(double));
	buffer = malloc(i_max * sizeof(double));

    if (old == NULL || current == NULL || next == NULL) {
        fprintf(stderr, "Could not allocate enough memory, aborting.\n");
        return EXIT_FAILURE;
    }

    memset(old, 0, i_max * sizeof(double));
    memset(current, 0, i_max * sizeof(double));
    memset(next, 0, i_max * sizeof(double));

    /* How should we will our first two generations? This is determined by the
     * optional further commandline arguments.
     * */
    if (argc > 3) {
        if (strcmp(argv[3], "sin") == 0) {
            fill(old, 1, i_max/4, 0, 2*3.14, sin);
            fill(current, 2, i_max/4, 0, 2*3.14, sin);
        } else if (strcmp(argv[3], "sinfull") == 0) {
            fill(old, 1, i_max-2, 0, 10*3.14, sin);
            fill(current, 2, i_max-3, 0, 10*3.14, sin);
        } else if (strcmp(argv[3], "gauss") == 0) {
            fill(old, 1, i_max/4, -3, 3, gauss);
            fill(current, 2, i_max/4, -3, 3, gauss);
        } else if (strcmp(argv[3], "file") == 0) {
            if (argc < 6) {
                printf("No files specified!\n");
                return EXIT_FAILURE;
            }
            file_read_double_array(argv[4], old, i_max);
            file_read_double_array(argv[5], current, i_max);
        } else {
            printf("Unknown initial mode: %s.\n", argv[3]);
            return EXIT_FAILURE;
        }
    } else {
        /* Default to sinus. */
        fill(old, 1, i_max/4, 0, 2*3.14, sin);
        fill(current, 2, i_max/4, 0, 2*3.14, sin);
    }
	
	
    

	rc = MPI_INIT(&argc, &argv);
	
	if (rc != MPI_SUCCESS)
    {
        fprintf(stderr, "Unable to set up MPI");
        MPI_Abort(MPI_COMM_WORLD, rc);    
    }
	
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank);
	MPI_Comm_size( MPI_COMM_WORLD, &size);
	
	if(myrank == 0)
	{
		timer_start();
	}
	
    /* Call the actual simulation that should be implemented in simulate.c. */
    ret = simulate(i_max, t_max, old, current, next);
		
    if(myrank == 0)
	{
		for (i=1; i < size){
			int pos1, pos2, t;
			t = i_max  / num_threads;
			pos1 = t * (i + 1) - t + 1;
			pos2 = t * (i + 1) ;
			MPI_Recv(buffer, i_max, MPI_Double, i, ((i + 1) * 10 + 1), MPI_COMM_WORLD, &status);
			for (; pos1 <= pos2 ; pos1++)
			{
				current[pos1] = buffer[pos1];
			}
			
		}
		time = timer_end();
		printf("Took %g seconds\n", time);
		printf("Normalized: %g seconds\n", time / (1. * i_max * t_max));
	} else {
		MPI_send(current, i_max, MPI_Double, i, ((i + 1) * 10 + 1), MPI_COMM_WORLD);
	}
	
	
	
    free(old);
    free(current);
    free(next);
	
	MPI_Finalize();
    return EXIT_SUCCESS;
}
