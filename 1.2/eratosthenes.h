/*
 * Concurrency en parallel Programmeren 2014/2015
 * NAME: Mehmet Faruk & Matthijs Klijn
 *
 * Assignment 1.2: Sieve of Eratosthenes
 * File: eratosthemes.h
 */

#pragma once
#define BUFFERSIZE 1000

struct buffer {
	int naturalNum[BUFFERSIZE];
	int filled;
	int in;
	int out;
	pthread_mutex_t lock;
	pthread_cond_t signal1; //Signal for the generator
	pthread_cond_t signal2; 
};

void setup(void);
struct buffer *initializeBuffer(void);
void generator(struct buffer* bffr);
void *filter(void *args);

