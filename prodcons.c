/*
 *  prodcons module
 *  Producer Consumer module
 *
 *  Implements routines for the producer consumer module based on
 *  chapter 30, section 2 of Operating Systems: Three Easy Pieces
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *
 *  Hassan Farhat and Avinash Bavisetty
 */

// Include only libraries for this module
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "counter.h"
#include "matrix.h"
#include "pcmatrix.h"
#include "prodcons.h"


Matrix ** bigmatrix;

// Define Locks, Condition variables, and so on here
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // declare/init a lock
pthread_cond_t full = PTHREAD_COND_INITIALIZER; // declare/init a CV
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; // declare/init a CV
pthread_cond_t boundedbuffer = PTHREAD_MUTEX_INITIALIZER; // declare/init a lock


int countIn = 0;
int countOut = 0;
int count = 0;

// Bounded buffer put() get()
int put(Matrix * value)
{
    pthread_mutex_lock(&mutex); // Lock the mutex for thread safety

    // Check if the buffer is full
    while (count >= BOUNDED_BUFFER_SIZE) {
        pthread_cond_wait(&empty, &mutex);
    }

    // Put the value into the buffer
    bigmatrix[countIn] = value;
    countIn = (countIn + 1) % BOUNDED_BUFFER_SIZE;
    count++;

    //Buffer is full
    pthread_cond_signal(&full);

    // Unlock the mutex
    pthread_mutex_unlock(&mutex);

    return EXIT_SUCCESS;
}

Matrix * get()
{
  return EXIT_SUCCESS;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
  return EXIT_SUCCESS;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
  return EXIT_SUCCESS;
}
