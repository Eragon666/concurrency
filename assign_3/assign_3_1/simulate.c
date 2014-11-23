/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"
#include "simulate.h"


/* Add any global variables you may need. */


/* Add any functions you may need (like a worker) here. */


/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, double *old_array,
        double *current_array, double *next_array)
{

	int i, t, start, end, myrank, num_threads;
	MPI_Comm_size( MPI_COMM_WORLD, &num_threads);
	MPI_Comm_rank( MPI_COMM_WORLD, &myrank);
	double *temp = NULL;
	double send1, send2, recv1, recv2;
	
	
	t = i_max  / num_threads;
	start = t * (myrank + 1) - t + 1;
	end = t * (myrank + 1) ;
	
	if(myrank == (num_threads - 1))
	{
		end--;
	}
	
	MPI_Status status
	
	for(t = 0; t < t_max; t++)
	{
		for(i = start; i <= end; i++)
		{
			next_array[i] = (2 * current_array[i]) - old_array[i] +  0.15 * (current_array[i-1] - (2 * current_array[i] - current_array[i+1]));
		}
		
		if(myrank == 0){
			send2 = next_array[end];
			MPI_Isend(send2, 1, MPI_DOUBLE, 1, 12, MPI_COMM_WORLD);
			MPI_Recv(recv2, 1, MPI_DOUBLE, 0, 21, MPI_COMM_WORLD, &status);
			next_array[end +1] = recv2;
		}
		else if(myrank == num_threads -1)
		{
			send1 = next_array[start];
			MPI_Isend(send1, 1, MPI_DOUBLE, (myrank - 1), ((myrank + 1) * 10 + myrank), MPI_COMM_WORLD);			
			MPI_Recv(recv1, 1, MPI_DOUBLE, 0, (myrank -1), ((myrank) * 10 + myrank + 1), MPI_COMM_WORLD, &status);		
			next_array[start -1] = recv;
		}
		else
		{
			send1 = next_array[start];
			send2 = next_array[end];
			MPI_Isend(send1, 1, MPI_DOUBLE, (myrank - 1), ((myrank + 1) * 10 + myrank), MPI_COMM_WORLD);
			MPI_Isend(send2, 1, MPI_DOUBLE, (myrank + 1), ((myrank + 1) * 10 + myrank + 2), MPI_COMM_WORLD);
			MPI_Recv(recv1, 1, MPI_DOUBLE, 0, (myrank -1), ((myrank) * 10 + myrank + 1), MPI_COMM_WORLD, &status);		
			MPI_Recv(recv2, 1, MPI_DOUBLE, 0, (myrank + 1), ((myrank + 2) * 10 + myrank + 1), MPI_COMM_WORLD, &status);
			next_array[start -1] = recv1;
			next_array[end +1] = recv2;
		}
		
		temp = old_array;
		old_array = current_array;
		current_array = next_array;
		next_array = temp;
	}
        
    /*
     * Your implementation should go here.
     */

    /* You should return a pointer to the array with the final results. */
    return current_array;
}
