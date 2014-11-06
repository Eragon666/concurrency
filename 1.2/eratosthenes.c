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

/* Setup the buffer and other variabeles for running the algorithm */
void setup() {
	struct buffer *bffr;

	bffr = initializeBuffer();	

	pthread_t firstThread;
 	
	/*pthread_cond_t signal1 = PTHREAD_COND_INITIALIZER;
	pthread_cond_t signal2 = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	
	bffr.filled = 0;
	bffr.in = 0;
	bffr.out = 0;
	bffr.lock = lock;
	bffr.signal1 = signal1; 
	bffr.signal2 = signal2;*/
	
	pthread_create(&firstThread, NULL, &filter, bffr);	

 	//pthread_join(firstThread, &results);

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
	bffr->signal1 = signal1; 
	bffr->signal2 = signal2;

	return bffr;
}

/* The generator generates natural numbers and outputs them to a output queue. */
void generator(struct buffer* bffr) {
	int i = 2;
	do {
		//Enter critical zone, trying to lock
		pthread_mutex_lock(&bffr->lock); 
		while(!(bffr->filled < BUFFERSIZE)) {
			//printf("Buffer is filled");
			//exit(1);
			pthread_cond_wait(&bffr->signal1, &bffr->lock);
		}

		bffr->naturalNum[bffr->in] = i;
		bffr->in = (bffr->in + 1) % BUFFERSIZE;
		bffr->filled = bffr->filled + 1;	

		//exit critical zone, unlock struct	
		pthread_cond_signal(&bffr->signal2);
		pthread_mutex_unlock(&bffr->lock);
		//printf("%d", i);
		i++;
	} while (1);
}

/* The filter has two distinct queues, one for receiving natural numbers as prime number candiiates
 * and the other for sending numbers which are not multiples of the filter thread's filter number, those
 * numbers are still potential prime numbers
 */
void *filter(void *args) {
	int prime;
	int primeCandidate;

	//printf("HELLO THERE");

	struct buffer *buffer;
	buffer = (struct buffer *) args;

	pthread_mutex_lock(&buffer->lock); 

	while(!(buffer->filled > 0)) {
		pthread_cond_wait(&buffer->signal2, &buffer->lock);
	}

	prime = buffer->naturalNum[buffer->out];
	buffer->out = (buffer->out +1) % BUFFERSIZE;
	buffer->filled = buffer->filled -1;
	
	pthread_cond_signal(&buffer->signal1);
 
	pthread_mutex_unlock(&buffer->lock);

	printf("\nPRIME FOUND:%d\n ", prime);

	struct buffer *newBuffer;

	newBuffer = initializeBuffer();
	pthread_t nextThread;

	pthread_create(&nextThread, NULL, &filter, newBuffer);

	do {
		pthread_mutex_lock(&buffer->lock);

		while(!(buffer->filled > 0)) {
            pthread_cond_wait(&buffer->signal2, &buffer->lock);
        }
        primeCandidate = buffer->naturalNum[buffer->out];
        buffer->out = (buffer->out + 1) % BUFFERSIZE;
        buffer->filled = buffer->filled - 1;

        pthread_cond_signal(&buffer->signal1);
        pthread_mutex_unlock(&buffer->lock);

        /* Check if the candidate is a multiple of the threads prime number. If
         * not, the candidate is put in the buffer for the next filter thread.
         */
        if(primeCandidate % prime != 0) {

            pthread_mutex_lock(&newBuffer->lock);
            while(!(newBuffer->filled < BUFFERSIZE)) {
                pthread_cond_wait(&newBuffer->signal1, &newBuffer->lock);
            }
            newBuffer->naturalNum[newBuffer->in] = primeCandidate;
            newBuffer->in = (newBuffer->in + 1) % BUFFERSIZE;
            newBuffer->filled = newBuffer->filled + 1;
            pthread_cond_signal(&newBuffer->signal2);
            pthread_mutex_unlock(&newBuffer->lock);
        }
		
	} while (1);

	return 0;
}

int main(int argc, char *argv[]) {
	setup();
	return 0;
}
