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


// Define Locks, Condition variables, and so on here
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // declare/init a lock
pthread_cond_t full = PTHREAD_COND_INITIALIZER; // declare/init a CV
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; // declare/init a CV
pthread_mutex_t boundedbuffer = PTHREAD_MUTEX_INITIALIZER; // declare/init a lock


Matrix ** bigmatrix;

// int in = 0;
// int out = 0;
// int counter = 0;

// counter_t procount;
// counter_t concount;






counter_t proCount;
counter_t conCount;

// Properties

volatile int countIn = 0; 
volatile int countOut = 0; 
volatile int count = 0; 


// Bounded buffer put() get()
int put(Matrix * value)
{
  	pthread_mutex_lock(&boundedbuffer);
  	if(value != NULL && count < BOUNDED_BUFFER_SIZE) {
      	bigmatrix[countIn] = value;
      	countOut = countIn;
      	countIn = (countIn + 1) % BOUNDED_BUFFER_SIZE;
      	count++;
        pthread_mutex_unlock(&boundedbuffer);
  	    return EXIT_SUCCESS;
  	}

  	pthread_mutex_unlock(&boundedbuffer);
  	return EXIT_FAILURE;


}

Matrix * get()
{
        pthread_mutex_lock(&boundedbuffer);
	    Matrix * matrix = NULL;

    	if(count <= 0) {
            pthread_mutex_unlock(&boundedbuffer);
      		return matrix;
    	} else {
			matrix = bigmatrix[countOut];
      		countIn = countOut;
      		countOut = (countOut - 1) % BOUNDED_BUFFER_SIZE;
      		count--;
		}

    	pthread_mutex_unlock(&boundedbuffer);
    	return matrix;

}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg) {
    ProdConsStats *prodConsStats = (ProdConsStats*) arg;
    while(get_cnt(&proCount) < NUMBER_OF_MATRICES) {
      	pthread_mutex_lock(&mutex);
        while (count == BOUNDED_BUFFER_SIZE && get_cnt(&proCount) < NUMBER_OF_MATRICES) {
            pthread_cond_wait(&empty, &mutex);
        }
        if (get_cnt(&proCount) >= NUMBER_OF_MATRICES) {
            pthread_cond_signal(&full);
            pthread_mutex_unlock(&mutex);
            return EXIT_SUCCESS;
        }
        Matrix *matrix = GenMatrixRandom();
        if (get_cnt(&proCount) < NUMBER_OF_MATRICES) {
            put(matrix);
            prodConsStats->matrixtotal += 1;
            prodConsStats->sumtotal += SumMatrix(matrix);
            increment_cnt(&proCount);
            pthread_cond_signal(&full);
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_cond_broadcast(&empty);
    return EXIT_SUCCESS;
}


// int canConsume() {
//     while(count == 0) {
//         if(get_cnt(&conCount) >= NUMBER_OF_MATRICES) {
//             pthread_cond_signal(&full);
//             pthread_mutex_unlock(&mutex);
//             return EXIT_SUCCESS;
//         }
//         pthread_cond_broadcast(&empty);
//         pthread_cond_wait(&full, &mutex);
//     }
//     return EXIT_FAILURE;
// }


// Matrix CONSUMER worker thread
void *cons_worker(void *arg) {

     ProdConsStats *prodConsStats = (ProdConsStats*) arg;

    int consume = {};

     while(get_cnt(&conCount) < NUMBER_OF_MATRICES) {
     	pthread_mutex_lock(&mutex);
     	
        while (count == 0) {
           if (get_cnt(&conCount) >= NUMBER_OF_MATRICES) {
               pthread_cond_signal(&full);
               pthread_mutex_unlock(&mutex);
               return EXIT_SUCCESS;
           }
           pthread_cond_broadcast(&empty);
           pthread_cond_wait(&full, &mutex);
       }

        Matrix * matrix = get();
	 	Matrix * matrixTwo = NULL;
     	Matrix * matrixThree;

        prodConsStats->sumtotal += SumMatrix(matrix);
	 	prodConsStats->matrixtotal += 1;
     	increment_cnt(&conCount);

     	do {
     	    while (count == 0) {
                if (get_cnt(&conCount) >= NUMBER_OF_MATRICES) {
                    pthread_cond_signal(&full);
                    pthread_mutex_unlock(&mutex);
                    return EXIT_SUCCESS;
                }
                pthread_cond_broadcast(&empty);
                pthread_cond_wait(&full, &mutex);
            }
         	if (matrixTwo != NULL) {
                	FreeMatrix(matrixTwo);
         	}
         	matrixTwo = get();
         	prodConsStats->sumtotal += SumMatrix(matrixTwo);
       		increment_cnt(&conCount);
       		prodConsStats->matrixtotal += 1;
         	matrixThree = MatrixMultiply(matrix, matrixTwo);
         } while (matrixThree == NULL);

     	DisplayMatrix(matrix, stdout);
     	printf("    x \n");
     	DisplayMatrix(matrixTwo, stdout);
     	printf("    = \n");
     	DisplayMatrix(matrixThree, stdout);
     	printf("\n");

     	FreeMatrix(matrix);
     	FreeMatrix(matrixTwo);
     	FreeMatrix(matrixThree);

     	prodConsStats->multtotal += 1;
     	pthread_cond_signal(&empty);
     	pthread_mutex_unlock(&mutex);
     	}
     	pthread_cond_broadcast(&full);
     	return EXIT_SUCCESS;










//    ProdConsStats *prodConsStats = (ProdConsStats*) arg;

//    while (get_cnt(&conCount) < NUMBER_OF_MATRICES) {
//        pthread_mutex_lock(&mutex);

//        while (count == 0) {
//            if (get_cnt(&conCount) >= NUMBER_OF_MATRICES) {
//                pthread_cond_signal(&full);
//                pthread_mutex_unlock(&mutex);
//                return EXIT_SUCCESS;
//            }
//            pthread_cond_broadcast(&empty);
//            pthread_cond_wait(&full, &mutex);
//        }

//        Matrix *matrix = get();
//        Matrix *matrixTwo = NULL;
//        Matrix *matrixThree;

//        prodConsStats->sumtotal += SumMatrix(matrix);
//        prodConsStats->matrixtotal += 1;
//        increment_cnt(&conCount);

//        if (count == 0 && get_cnt(&conCount) >= NUMBER_OF_MATRICES) {
//            pthread_cond_signal(&full);
//            pthread_mutex_unlock(&mutex);
//            return EXIT_SUCCESS;
//        }

//        do {
//            if (count == 0 && get_cnt(&conCount) >= NUMBER_OF_MATRICES) {
//                pthread_cond_signal(&full);
//                pthread_mutex_unlock(&mutex);
//                return EXIT_SUCCESS;
//            }

//            if (matrixTwo != NULL) {
//                FreeMatrix(matrixTwo);
//            }
//            matrixTwo = get();
//            prodConsStats->sumtotal += SumMatrix(matrixTwo);
//            increment_cnt(&conCount);
//            prodConsStats->matrixtotal += 1;

//            matrixThree = MatrixMultiply(matrix, matrixTwo);
//        } while (matrixThree == NULL);

//        DisplayMatrix(matrix, stdout);
//        printf("    x \n");
//        DisplayMatrix(matrixTwo, stdout);
//        printf("    = \n");
//        DisplayMatrix(matrixThree, stdout);
//        printf("\n");

//        FreeMatrix(matrix);
//        FreeMatrix(matrixTwo);
//        FreeMatrix(matrixThree);

//        prodConsStats->multtotal += 1;
//        pthread_cond_signal(&empty);
//        pthread_mutex_unlock(&mutex);
//    }

//    pthread_cond_broadcast(&full);
//    return EXIT_SUCCESS;

}

