/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "simulate.h"

/* Add any global variables you may need. */

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t start_line = PTHREAD_COND_INITIALIZER;
pthread_cond_t swap_arrays = PTHREAD_COND_INITIALIZER;

typedef struct order {
    double *old;
    double *current;
    double *next;
    int semaphore;
    int boole;
    int size;
};

typedef struct list { 
    int nt;
    int start;
    struct order arrays;
};

/* Add any functions you may need (like a worker) here. */
void update_rotation(double **old, double **current, double **next){
  double *temp = *old;
  *old = *current;
  *current = *next;
  *next = temp;
}

/* temp1 = (2 * current_array[i]) - old_array[i];
 * temp2 = 0.15 * (current_array[i-1] - (2 * current_array[i] - current_array[i+1]));
 * next_array[i] = temp1 + temp2;
 */

void *compute(void *arguments)
{
    struct list *args;
    args = (struct list *) arguments    
    
    int i;   
    while(args->arrays->boole)
    { 
        
        for(i = args->start; i < args->arrays->size-1; i += args->nt) 
        {
            temp1 = (2 * current_array[i]) - old_array[i];
            temp2 = 0.15 * (current_array[i-1] - (2 * current_array[i] - current_array[i+1]));
            next_array[i] = temp1 + temp2;  
        }
        
        pthread_mutex_lock(&count_mutex)
        if(args->arrays->semaphore > 1)
        { 
            args->arrays->semaphore = args->order->semaphore-1;
            pthread_cond_wait(&start_line ,&count_mutex);
        } 
        else if (args->arrays->semaphore == 1)
        { 
            args->arrays->semaphore = args->nt;
            pthread_cond_signal(&swap_arrays);
            pthread_cond_wait(&start_line ,&count_mutex);
            
        }
        pthread_mutex_unlock(&count_mutex)
        
        
    }
}

/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * num_threads: how many threads to use (excluding the main threads)
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{
    
    double temp1;
    double temp2;
    int tc;
    struct list tlist[num_threads];
    struct order tarray;

    tarray.old = *old_array;
    tarray.current = *current_array;
    tarray.next = *next_array
    tarray.semaphore = 0;
    tarray.boole = 1;
    tarray.size = i_max;
    
    for (tc = 0; tc < num_threads; tc++)
    {
        tlist[tc].nt = num_threads;
        tlist[tc].start = tc + 1;
        tlist[tc].arrays = &tarray; 
    }

    pthread_t thread_ids[num_threads];
    pthread_mutex_lock( &count_mutex);
    
    for (tc = 0; tc < num_threads; tc++)
    {
        pthread_create( &thread_ids[tc],
                        NULL,
                        &Compute,
                        NULL);               

    }
    int i;
    int count;    
    
    
    for(count = 0; count < t_max; count++ )   
    {
        //pthread_mutex_lock( &count_mutex);
        pthread_cond_wait( &swap_arrays, &count_mutex);
         

        // printf("%d:  %f - %f - %f\n", count, old_array[50000], current_array[50000], next_array[50000]);
        /*
        * After each timestep, you should swap the buffers around. Watch out none
        * of the threads actually use the buffers at that time.
        */
        
        update_rotation(&tarray->old, &tarray->current, &tarray->next);
        
        pthread_cond_signal(& start_line);
        /*tempray = *old_array;
        *old_array = *current_array;
        *current_array = *next_array;
        *next_array = tempray;*/
    }
    /* You should return a pointer to the array with the final results. */
    return current_array;
}
