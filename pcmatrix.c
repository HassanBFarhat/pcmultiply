/*
 *  pcmatrix module
 *  Primary module providing control flow for the pcMatrix program
 *
 *  Producer consumer bounded buffer program to produce random matrices in parallel
 *  and consume them while searching for valid pairs for matrix multiplication.
 *  Matrix multiplication requires the first matrix column count equal the
 *  second matrix row count.
 *
 *  A matrix is consumed from the bounded buffer.  Then matrices are consumed
 *  from the bounded buffer, ONE AT A TIME, until an eligible matrix for multiplication
 *  is found.
 *
 *  Totals are tracked using the ProdConsStats Struct for each thread separately:
 *  - the total number of matrices multiplied (multtotal from each consumer thread)
 *  - the total number of matrices produced (matrixtotal from each producer thread)
 *  - the total number of matrices consumed (matrixtotal from each consumer thread)
 *  - the sum of all elements of all matrices produced and consumed (sumtotal from each producer and consumer thread)
 *  
 *  Then, these values from each thread are aggregated in main thread for output
 *
 *  Correct programs will produce and consume the same number of matrices, and
 *  report the same sum for all matrix elements produced and consumed.
 *
 *  Each thread produces a total sum of the value of
 *  randomly generated elements.  Producer sum and consumer sum must match.
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include "matrix.h"
#include "counter.h"
#include "prodcons.h"
#include "pcmatrix.h"

int main (int argc, char * argv[])
{
  // Process command line arguments
  int numw = NUMWORK;
  if (argc==1)
  {
    BOUNDED_BUFFER_SIZE=MAX;
    NUMBER_OF_MATRICES=LOOPS;
    MATRIX_MODE=DEFAULT_MATRIX_MODE;
    printf("USING DEFAULTS: worker_threads=%d bounded_buffer_size=%d matricies=%d matrix_mode=%d\n",numw,BOUNDED_BUFFER_SIZE,NUMBER_OF_MATRICES,MATRIX_MODE);
  }
  else
  {
    if (argc==2)
    {
      numw=atoi(argv[1]);
      BOUNDED_BUFFER_SIZE=MAX;
      NUMBER_OF_MATRICES=LOOPS;
      MATRIX_MODE=DEFAULT_MATRIX_MODE;
    }
    if (argc==3)
    {
      numw=atoi(argv[1]);
      BOUNDED_BUFFER_SIZE=atoi(argv[2]);
      NUMBER_OF_MATRICES=LOOPS;
      MATRIX_MODE=DEFAULT_MATRIX_MODE;
    }
    if (argc==4)
    {
      numw=atoi(argv[1]);
      BOUNDED_BUFFER_SIZE=atoi(argv[2]);
      NUMBER_OF_MATRICES=atoi(argv[3]);
      MATRIX_MODE=DEFAULT_MATRIX_MODE;
    }
    if (argc==5)
    {
      numw=atoi(argv[1]);
      BOUNDED_BUFFER_SIZE=atoi(argv[2]);
      NUMBER_OF_MATRICES=atoi(argv[3]);
      MATRIX_MODE=atoi(argv[4]);
    }
    printf("USING: worker_threads=%d bounded_buffer_size=%d matricies=%d matrix_mode=%d\n",numw,BOUNDED_BUFFER_SIZE,NUMBER_OF_MATRICES,MATRIX_MODE);
  }

  time_t t;
  // Seed the random number generator with the system time
  srand((unsigned) time(&t));

  printf("Producing %d matrices in mode %d.\n",NUMBER_OF_MATRICES,MATRIX_MODE);
  printf("Using a shared buffer of size=%d\n", BOUNDED_BUFFER_SIZE);
  printf("With %d producer and consumer thread(s).\n",numw);
  printf("\n");

  // Here is an example to define one producer and one consumer
  // pthread_t pr;
 // pthread_t co;

    // Add your code here to create threads and so on

    // Initialize buffer
    bigmatrix = (Matrix **)malloc(sizeof(Matrix *) * BOUNDED_BUFFER_SIZE);
    if (bigmatrix == NULL) {
        perror("Failed to allocate memory for buffer");
        exit(EXIT_FAILURE);
    }
    printf("Buffer created\n");

    // Initialize producer and consumer statistics
    ProdConsStats *consumer_stats = (ProdConsStats *)malloc(sizeof(ProdConsStats));
    ProdConsStats *producer_stats = (ProdConsStats *)malloc(sizeof(ProdConsStats));
    if (consumer_stats == NULL || producer_stats == NULL) {
        perror("Failed to allocate memory for statistics");
        exit(EXIT_FAILURE);
    }

    ProdConsStats *consumer = (ProdConsStats *) malloc(sizeof(ProdConsStats));
    consumer_stats->sumtotal = 0;
    consumer_stats->multtotal = 0;
    consumer_stats->matrixtotal = 0;

    ProdConsStats *producer = (ProdConsStats *) malloc(sizeof(ProdConsStats));
    producer_stats->sumtotal = 0;
    producer_stats->multtotal = 0;
    producer_stats->matrixtotal = 0;

    // Define thread attributes
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0) {
        perror("Failed to initialize thread attributes");
        exit(EXIT_FAILURE);
    }

    // Create multiple producer and consumer threads
    pthread_t producers[numw];
    pthread_t consumers[numw];

    for (int i = 0; i < numw; i++) {
        if (pthread_create(&producers[i], &attr, prod_worker, producer_stats) != 0) {
            perror("Failed to create producer thread");
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&consumers[i], &attr, cons_worker, consumer_stats) != 0) {
            perror("Failed to create consumer thread");
            exit(EXIT_FAILURE);
        }
    }
    printf("Producer and consumer threads created\n");

    // Join producer and consumer threads when they are done
    for (int i = 0; i < numw; i++) {
        if (pthread_join(producers[i], NULL) != 0) {
            perror("Failed to join producer thread");
            exit(EXIT_FAILURE);
        }
        if (pthread_join(consumers[i], NULL) != 0) {
            perror("Failed to join consumer thread");
            exit(EXIT_FAILURE);
        }
    }
    printf("Producer and consumer threads joined\n");


  // These are used to aggregate total numbers for main thread output
    int prs = producer->sumtotal;
    int cos = consumer->sumtotal;
    int prodtot = producer->matrixtotal;
    int constot = consumer->matrixtotal;
    int consmul = consumer->multtotal;

  // consume ProdConsStats from producer and consumer threads [HINT: return from join]
  // add up total matrix stats in prs, cos, prodtot, constot, consmul

  printf("Sum of Matrix elements --> Produced=%d = Consumed=%d\n",prs,cos);
  printf("Matrices produced=%d consumed=%d multiplied=%d\n",prodtot,constot,consmul);

  free(consumer_stats);
  free(producer_stats);
  free(bigmatrix);

  return 0;
}