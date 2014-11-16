#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "omp.h"
#include "timer.h"

typedef double (*func_t)(double x);

double sum (double* vec, int len)
{
	int i;
	double accu = 0.0;
	
	#pragma omp parallel for reduction(*:accu)
	for (i=0; i<len; i++)
	{
		accu = accu + vec[i];
	}
	
	return accu;
}

double reduce (double (fun) (double, double), 
						double* vec, int len, double neutral)
{
	int i;
	double accu = neutral;
	
	#pragma omp parallel for
	for ( i=0; i<len; i++) 
	{
		#pragma omp critical
		accu = fun( accu, vec[i]);
	}
	
	return accu;
}

// Reused fill code from assign2_1.c
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


double add_dbls(double x, double y)
{
	return x+y;
}

double reduce_wrapper(double * vec, int len) {
    return reduce(add_dbls, vec, len, 0.0);
}

void experiment(double(fun)(double*, int), char* ver)
{
	
	double *vecarr;
	double time;
	
	int num_threads[] = {1, 2, 4, 6, 8};
	int vector_size[] = {1000, 10000, 100000, 1000000, 10000000};
	int i, j;
	
	printf("#----- %s -----\n", ver);
	
	for  ( i = 0; i < 5; i++)
	{
		vecarr = malloc(vector_size[i]*sizeof(double));
		fill(vecarr, 1, vector_size[i], 0, 2*3.14, sin);		
		
		for ( j = 0; j < 5; j++)
		{	
			omp_set_num_threads(num_threads[j]);
			
			
			timer_start();
			
			fun(vecarr, vector_size[i]);
			
			time = timer_end();
			
			printf("Number of threads : %d, Vector size%d. it took %g seconds\n", num_threads[j], vector_size[i], time);
		}
		
		free(vecarr);
	}
}

int main(int argc, char const *argv[])
{
    experiment(sum, "sum");
    experiment(reduce_wrapper, "reduce");

    return 0;
}
