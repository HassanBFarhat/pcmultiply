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

counter_t count1;

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
    pthread_mutex_lock(&mutex); // Lock the mutex for thread safety

    // Check if the buffer is empty
    while (count <= 0) {
        pthread_cond_wait(&full, &mutex);
    }

    // Get the value from the buffer
    Matrix *matrix = bigmatrix[countOut];
    countOut = (countOut - 1 + BOUNDED_BUFFER_SIZE) % BOUNDED_BUFFER_SIZE;
    count--;

    // Signal that the buffer is not full anymore
    pthread_cond_signal(&empty);

    // Unlock the mutex
    pthread_mutex_unlock(&mutex);

    return matrix;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg) {
    ProdConsStats *prodConsStats = (ProdConsStats*)arg;

    while (1) {
        //mutex is locked
        pthread_mutex_lock(&mutex);

        // check to see if the matrices count is sufficient
        if (get_cnt(&count1) >= NUMBER_OF_MATRICES) {

            //mutex is unlocked
            pthread_mutex_unlock(&mutex);

            // it will be broadcasted that it is no longer empty
            pthread_cond_broadcast(&empty);
            return EXIT_SUCCESS;
        }

        // Check if the buffer is full, and if its full wait till its not full
        while (count == BOUNDED_BUFFER_SIZE) {

            pthread_cond_wait(&empty, &mutex);
        }

        // matrix is generated
        Matrix *matrix = GenMatrixRandom();

        // matrix is put into the buffer
        put(matrix);

        // update
        prodConsStats->matrixtotal += 1;
        prodConsStats->sumtotal += SumMatrix(matrix);
        increment_cnt(&count1);

        //not empty anymore
        pthread_cond_signal(&full);

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg) {
    ProdConsStats *prodConsStats = (ProdConsStats*)arg;

    while (1) {
        pthread_mutex_lock(&mutex); // Lock the mutex

        // Check if enough matrices have been consumed
        if (get_cnt(&count1) >= NUMBER_OF_MATRICES) {
            pthread_mutex_unlock(&mutex); // Unlock the mutex before returning
            return EXIT_SUCCESS; // Exit the thread
        }

        // Check if the buffer is empty
        while (count <= 0) {
            pthread_cond_wait(&full, &mutex); // Wait until the buffer is not empty
        }

        // Get the first matrix from the buffer
        Matrix *matrix1 = get();

        // Check if we received a NULL matrix (no more matrices available)
        if (matrix1 == NULL) {
            pthread_mutex_unlock(&mutex); // Unlock the mutex before returning
            return EXIT_SUCCESS; // Exit the thread
        }

        // Get the second matrix from the buffer
        Matrix *matrix2 = get();

        // Check if we received a NULL matrix (no more matrices available)
        if (matrix2 == NULL) {
            FreeMatrix(matrix1); // Free the first matrix
            pthread_mutex_unlock(&mutex); // Unlock the mutex before returning
            return EXIT_SUCCESS; // Exit the thread
        }

        // Perform matrix multiplication
        Matrix *result_matrix = MatrixMultiply(matrix1, matrix2);

        // Check if multiplication was successful
        if (result_matrix != NULL) {
            // Update statistics
            prodConsStats->multtotal += 1;
            DisplayMatrix(result_matrix, stdout); // Display the result matrix
            FreeMatrix(matrix1); // Free the input matrices
            FreeMatrix(matrix2);
            FreeMatrix(result_matrix);
            increment_cnt(&count1); // Increment the number of matrices consumed
        } else {
            // If multiplication failed, discard the input matrices
            FreeMatrix(matrix1);
            FreeMatrix(matrix2);
        }

        pthread_cond_signal(&empty); // Signal that the buffer is not full anymore
        pthread_mutex_unlock(&mutex); // Unlock the mutex
    }
}

