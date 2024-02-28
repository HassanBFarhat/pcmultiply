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

int in = 0;
int out = 0;
int counter = 0;

counter_t procount;
counter_t concount;

// Bounded buffer put() get()
int put(Matrix * value)
{

    // pthread_mutex_lock(&boundedbuffer); // Lock the mutex for thread safety

    // // if the buffer is not full then the count is incremented
    // if (counter < BOUNDED_BUFFER_SIZE && value != NULL) {
    //     bigmatrix[in] = value;
    //     in = (in+1) % BOUNDED_BUFFER_SIZE;
    //     counter++;
    // } else {
    //     pthread_cond_wait(&empty, &mutex);
    // }

    // //Buffer is full
    // pthread_cond_signal(&full);

    // // Unlock the mutex
    // pthread_mutex_unlock(&boundedbuffer);

    // return 0;











  	pthread_mutex_lock(&bufferLock);
    // if its not null and is less thaen the count

  	if(value != NULL && count < BOUNDED_BUFFER_SIZE) {
      	bigmatrix[countIn] = value;
      	countOut = countIn;
      	countIn = (countIn + 1) % BOUNDED_BUFFER_SIZE;
      	count++;
  	}

  	pthread_mutex_unlock(&bufferLock);
	//printf("PUT END\n");
  	return EXIT_SUCCESS;


}

Matrix * get()
{
    // pthread_mutex_lock(&boundedbuffer); // Lock the mutex for thread safety

    // // Wait while the buffer is empty
    // while (counter == 0) {
    //     pthread_cond_wait(&empty, &boundedbuffer);
    // }

    // // Get the matrix from the buffer
    // Matrix *temp = bigmatrix[out];
    // out = (out + 1) % BOUNDED_BUFFER_SIZE;
    // counter--;

    // // Signal that the buffer is not full anymore
    // pthread_cond_signal(&full);

    // // Unlock the mutex
    // pthread_mutex_unlock(&boundedbuffer);

    // return temp;











    //printf("GET START\n");
	// Properties
	Matrix * matrix = NULL;
    	pthread_mutex_lock(&bufferLock);
    	// Get what is in the buffer if it is not empty

    	if(count <= 0) {
      		return (matrix != NULL)  ? matrix : NULL;
    	} else {
			matrix = bigmatrix[countOut];
      		countIn = countOut;
      		countOut = (countOut - 1) % BOUNDED_BUFFER_SIZE;
      		count--;
		}

    	pthread_mutex_unlock(&bufferLock);
		//printf("GET END\n");
    	return (matrix != NULL)  ? matrix : NULL;

}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg) {

    // ProdConsStats *pcs = (ProdConsStats *)arg; // Stats for the producer

    // while (get_cnt(&procount) < NUMBER_OF_MATRICES) // Loop until we produce the specified max
    // {
    //     pthread_mutex_lock(&mutex); // Lock the mutex

    //     if (counter >= BOUNDED_BUFFER_SIZE) // If the buffer is already full...
    //     {
    //         pthread_cond_wait(&empty, &mutex); // wait until it has room
    //     }

    //     Matrix *m = GenMatrixRandom(); // Generate a matrix
    //     put(m);						   // Put it in the buffer

    //     pcs->matrixtotal += 1;		   // Increment number of generated matricies
    //     pcs->sumtotal += SumMatrix(m); // Increment the total sum of produced matricies
    //     increment_cnt(&procount);  // Increment the produced counter

    //     pthread_mutex_unlock(&mutex); // Unlock the mutex
    // }

    // return EXIT_SUCCESS;









    // our produced and consumed stats
    ProdConsStats *prodConsStats = (ProdConsStats*) arg;
	// Produce new matrices while there is not enouggh.
    while(get_cnt(&proCount) < NUMBER_OF_MATRICES) {
    	// Lock
      	pthread_mutex_lock(&mutex);
		// If the buffer is full exit
      	while(count == BOUNDED_BUFFER_SIZE) {
      		// Check to see if the count is mmore 
        	if(get_cnt(&proCount) >= NUMBER_OF_MATRICES) {
          		pthread_cond_signal(&full);
          		pthread_mutex_unlock(&mutex);
          		return EXIT_SUCCESS;
          	}
           	// if not, it will wait
          	pthread_cond_wait(&empty, &mutex);
      	}
		//printf("End First while loop\n");
		// Generate a matrix
     	 Matrix * matrix = GenMatrixRandom();
		// Generate more matrices if needed.
      	if(get_cnt(&proCount) < NUMBER_OF_MATRICES) {

        	put(matrix);

			prodConsStats->matrixtotal += 1;
        	prodConsStats->sumtotal += SumMatrix(matrix);
        	increment_cnt(&proCount);
        	pthread_cond_signal(&full); 
      	}
      	// unlock
      	pthread_mutex_unlock(&mutex);
		//printf("MAIN WHILE LOOP\n");
    }
    // Exit
	//printf("END PROD\n");
    pthread_cond_broadcast(&empty);
    return EXIT_SUCCESS;


}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg) {

    // ProdConsStats *con = (ProdConsStats*)arg;
    // while (get_cnt(&concount) < NUMBER_OF_MATRICES) {

    //     //mutex gets locked
    //     pthread_mutex_lock(&mutex);

    //     // Check if enough matrices have been consumed
    //     if (counter <= 1) {
    //         if (get_cnt(&concount) >= NUMBER_OF_MATRICES) {
    //             pthread_cond_signal(&full);
    //             pthread_mutex_unlock(&mutex); // Unlock the mutex before returning
    //             return EXIT_SUCCESS; // Exit the thread
    //         }
    //         pthread_cond_broadcast(&empty);
    //         pthread_cond_wait(&full, &mutex);
    //     }

    //     // Get the first matrix from the buffer
    //     Matrix *matrix1 = get();

    //     // Check if we received a NULL matrix (no more matrices available)
    //     if (matrix1 == NULL) {
    //         pthread_mutex_unlock(&mutex); // Unlock the mutex before returning
    //         continue; // Move to the next iteration
    //     }

    //     // Get the second matrix from the buffer
    //     Matrix *matrix2 = get();

    //     // Check if we received a NULL matrix (no more matrices available)
    //     if (matrix2 == NULL) {
    //         FreeMatrix(matrix1); // Free the first matrix
    //         pthread_mutex_unlock(&mutex); // Unlock the mutex before returning
    //         continue; // Move to the next iteration
    //     }

    //     // Perform matrix multiplication if matrices are valid
    //     if (matrix1->cols == matrix2->rows) {
    //         con->sumtotal += SumMatrix(matrix1) + SumMatrix(matrix2);
    //         con->matrixtotal += 2;

    //         // Perform matrix multiplication
    //         Matrix *result_matrix = MatrixMultiply(matrix1, matrix2);

    //         // Check if multiplication was successful
    //         if (result_matrix != NULL) {
    //             // Update statistics
    //             con->multtotal += 1;
    //             DisplayMatrix(result_matrix, stdout); // Display the result matrix
    //             increment_cnt(&concount); // Increment the number of matrices consumed

    //             // Free the input matrices and the result matrix
    //             FreeMatrix(matrix1);
    //             FreeMatrix(matrix2);
    //             FreeMatrix(result_matrix);
    //         }
    //     }

    //     pthread_cond_signal(&empty); // Signal that the buffer is not full anymore
    //     pthread_mutex_unlock(&mutex); // Unlock the mutex
    // }

    // return EXIT_SUCCESS;













        // Properties
    ProdConsStats *prodConsStats = (ProdConsStats*) arg;
   // Loop while the matrix is less then the total number of matrices.
    while(get_cnt(&conCount) < NUMBER_OF_MATRICES) {
    	// Lock
    	pthread_mutex_lock(&mutex);
        //Check if there is a matrix to be consumed

    	if(canConsume() == EXIT_SUCCESS) return EXIT_SUCCESS;
        // Matrices
        Matrix * matrix = get();
		Matrix * matrixTwo = NULL;
    	Matrix * matrixThree;

        // Update fields
        prodConsStats->sumtotal += SumMatrix(matrix);
		prodConsStats->matrixtotal += 1;
    	increment_cnt(&conCount);
        // Check if there is a matrix to be consumed
    	if(canConsume() == EXIT_SUCCESS) return EXIT_SUCCESS;
        // Get a buffer that can be multiplied
    	do {
			// Check if there is a matrix to be consumed
    		if(canConsume() == EXIT_SUCCESS) return EXIT_SUCCESS;
            // if matrixTwo is null (no matrix), discard it and get the next one.
        	if (matrixTwo != NULL) {
               	FreeMatrix(matrixTwo);
        	}
        	matrixTwo = get(); 
        	// update stats
        	prodConsStats->sumtotal += SumMatrix(matrixTwo);
      		increment_cnt(&conCount);
      		prodConsStats->matrixtotal += 1; 
      		// multiple (matrix * matrixTwo) = matrixThree
        	matrixThree = MatrixMultiply(matrix, matrixTwo);
        } while (matrixThree == NULL);
    	// Display stats
    	DisplayMatrix(matrix, stdout);
		// Make space
    	printf("    x \n");
    	DisplayMatrix(matrixTwo, stdout);
		// Make space
    	printf("    = \n");
    	DisplayMatrix(matrixThree, stdout);
		// Make space
    	printf("\n");
    	// Free space in the matrices
    	FreeMatrix(matrix);
    	FreeMatrix(matrixTwo);
    	FreeMatrix(matrixThree);
		// update pointer
    	prodConsStats->multtotal += 1;
    	//Empty signal and unlocks here.
    	pthread_cond_signal(&empty);
    	pthread_mutex_unlock(&mutex);
    	}
    	// Exit
    	pthread_cond_broadcast(&full);
    	return EXIT_SUCCESS;




}


int canConsume() {
    while(count == 0) {
        if(get_cnt(&conCount) >= NUMBER_OF_MATRICES) {
            pthread_cond_signal(&full);
            pthread_mutex_unlock(&mutex);
        	return EXIT_SUCCESS; 
        }
        pthread_cond_broadcast(&empty);
        pthread_cond_wait(&full, &mutex);
    }
	return EXIT_FAILURE; 
}
