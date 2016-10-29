// Leszek Zychowski
// loosely based on Scatter-Reduce.c example

// suppress Visual Studio compiler warnings
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

#include "vector.h"

// define variables
vector v;

// define functions
void createVector(int* randomNumbers, int userInput);
int *randomNumberGenerator(int Num_Elements);
int computeProduct(int *array, int num_elements);

int main(int argc, char* argv[])
{
	int comm_sz, my_rank;
	int *randomNumbers = NULL;
	int count = NULL;
	int userInput = NULL;
	int *splitCount = NULL;
	int partialResult = 0;
	int finalResult;
	double geometricMean;

	// begin parallel work
	MPI_Init(NULL, NULL);

	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	// error handling
	if (my_rank == 0) {
		if (argv[1] == NULL) {
			printf("No user input provided for <N>.  Terminating program.  Please rerun with <P> specifying number of processes and <N> specifying number of integers.\n\n");
			printf("WINDOWS - mpiexec -n <P> Geo-Mean.exe <N>\n");
			printf("LINUX - mpiexec -n <P> Geo-Mean <N>\n\n");
			return 0;
		}
		else if (atoi(argv[1]) > 8) {
			printf("Maximum number of elements <N> can be 8.  Terminating program.  Please rerun with <P> specifying number of processes and <N> specifying number of integers.\n\n");
			printf("WINDOWS - mpiexec -n <P> Geo-Mean.exe <N>\n");
			printf("LINUX - mpiexec -n <P> Geo-Mean <N>\n\n");
			return 0;
		}
	}

	// read last parameter from the executed command
	userInput = atoi(argv[1]);

	// determine split size
	count = userInput / comm_sz;

	if (my_rank == 0) {

		// seed random generator with time
		srand(time(NULL));

		// disable output buffer
		setbuf(stdout, NULL);

		// ask for # of items
		printf("\n##############################\n### Geometric Mean Program ###\n##############################\n");

		// generate random numbers
		randomNumbers = randomNumberGenerator(userInput);

		// create and populate vector
		createVector(randomNumbers, userInput);
	}

	// pad vector with 1s which will not affect product function
	// this is to create a proper split number without remainder
	while ((userInput % comm_sz) != 0)
	{
		userInput++;
		vector_add(&v, 1);
	}

	// Generate a buffer for holding a subset of the entire array 
	splitCount = (int *)malloc(sizeof(int) * count);

	// Scatter the random integer numbers from process 0 to all processes 
	MPI_Scatter(randomNumbers, count, MPI_INT, splitCount, count, MPI_INT, 0, MPI_COMM_WORLD);

	// Compute the product value of a subset array on each process
	partialResult = computeProduct(splitCount, count);

	// Reduce the total product value of all elements to process 0
	MPI_Reduce(&partialResult, &finalResult, 1, MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);

	// print results
	if (my_rank == 0) {
		printf("\n\nThe final result is %d\n", finalResult);
		geometricMean = pow((double)finalResult, (1/(double)atoi(argv[1])));
		printf("The geometric mean is %lf\n", geometricMean);
	}

	MPI_Finalize();

	return 0;
}

// create the vector with random numbers
void createVector(int* randomNumbers, int userInput)
{
	vector_init(&v);

	for (int i = 0; i < userInput; i++) {
		vector_add(&v, randomNumbers[i]);
	}

	// output numbers in the vector
	printf("\nThe following %d numbers were generated:\n", userInput);
	for (int i = 0; i < userInput; i++) {
		printf(" %d ", vector_get(&v, i));
	}
}

// generates X crandom numbers from 1-10 specified by count
int *randomNumberGenerator(int count)
{
	int *randomNumbers = (int *)malloc(sizeof(int) * count);
	int i;

	for (i = 0; i < count; i++) {
		randomNumbers[i] = (rand() % 10) + 1;
	}

	return randomNumbers;
}

// computes the product of an array of numbers 
int computeProduct(int *array, int num_elements)
{
	int product = 1;
	int i;

	for (i = 0; i < num_elements; i++) {
		product *= array[i];
	}

	return product;
}
