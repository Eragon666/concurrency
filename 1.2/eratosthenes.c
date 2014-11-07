/*
 * Concurrency en parallel Programmeren 2014/2015
 * NAME: Mehmet Faruk & Matthijs Klijn
 *
 * Assignment 1.2: Sieve of Eratosthenes
 * File: eratosthemes.c
 */

#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include "eratosthenes.h"

time_t start, stop;

/* Setup the buffer and other variabeles for running the algorithm */
void setup() {
	struct buffer *bffr;

	bffr = initializeBuffer();	

	pthread_t firstThread;
	
	pthread_create(&firstThread, NULL, &filter, bffr);	

	generator(bffr);
}

struct buffer *initializeBuffer() {
	struct buffer *bffr = malloc(sizeof(struct buffer));

	pthread_cond_t signal1 = PTHREAD_COND_INITIALIZER;
	pthread_cond_t signal2 = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	
	bffr->filled = 0;
	bffr->in = 0;
	bffr->out = 0;
	bffr->lock = lock;
	bffr->signal1 = signal1; //Wake up producer 
	bffr->signal2 = signal2; //Wake up consumer

	return bffr;
}

/* The generator generates natural numbers and outputs them to a output queue. */
void generator(struct buffer* bffr) {
	int i = 2;
	do {
		//Enter critical zone, trying to lock
		pthread_mutex_lock(&bffr->lock); 

		//Wait until the buffer isn't fully occupied anymore
		while(!(bffr->filled < BUFFERSIZE)) {
			pthread_cond_wait(&bffr->signal1, &bffr->lock);
		}

		bffr->naturalNum[bffr->in] = i;
		bffr->in = (bffr->in + 1) % BUFFERSIZE;
		bffr->filled = bffr->filled + 1;	

		//exit critical zone, unlock struct	
		pthread_cond_signal(&bffr->signal2);
		pthread_mutex_unlock(&bffr->lock);

		i++;
	} while (1);
}

/* The filter has two distinct queues, one for receiving natural numbers as prime number candiiates
 * and the other for sending numbers which are not multiples of the filter thread's filter number, those
 * numbers are still potential prime numbers
 */
void *filter(void *args) {
	int primeTest, prime, created;

	created = 0;

	//Get buffer from *args and initialize a new buffer
	struct buffer *bffr, *newBffr;
	bffr = (struct buffer *) args;
	newBffr = initializeBuffer();

	//Enter critical zone
	pthread_mutex_lock(&bffr->lock); 

	while(!(bffr->filled > 0)) {
		pthread_cond_wait(&bffr->signal2, &bffr->lock);
	}

	//Get the prime number and print, use fprintf so it will print immidiately 
	prime = bffr->naturalNum[bffr->out];
	fprintf(stderr, "%d\n", prime);

	bffr->filled--;
	bffr->out = (bffr->out + 1) % BUFFERSIZE;
	
	pthread_cond_signal(&bffr->signal1);
 
	pthread_mutex_unlock(&bffr->lock);	
	
	do {
		//Enter critical zone
		pthread_mutex_lock(&bffr->lock);

		while(!(bffr->filled > 0)) {
            pthread_cond_wait(&bffr->signal2, &bffr->lock);
        }

		//Get the number to test 
        primeTest = bffr->naturalNum[bffr->out];
		
		bffr->filled--;
        bffr->out = (bffr->out + 1) % BUFFERSIZE;

        pthread_cond_signal(&bffr->signal1);
        pthread_mutex_unlock(&bffr->lock);

        //Check if the primeTest is a multiple of the prime number, if not add it to the next queue and
		//start a new thread for this number
        if(primeTest % prime != 0) {
			if (created == 0) {
				pthread_t nextThread;
				pthread_create(&nextThread, NULL, &filter, newBffr);
				created = 1;
			}

            pthread_mutex_lock(&newBffr->lock);
            while(!(newBffr->filled < BUFFERSIZE)) {
                pthread_cond_wait(&newBffr->signal1, &newBffr->lock);
            }

            newBffr->naturalNum[newBffr->in] = primeTest;
            newBffr->filled++;
			newBffr->in = (newBffr->in + 1) % BUFFERSIZE;

	        pthread_cond_signal(&newBffr->signal2);
            pthread_mutex_unlock(&newBffr->lock);
        }
		
	} while (1); 
}

int main(int argc, char *argv[]) {
	setup();
	time(&start);
	return 1;
}
