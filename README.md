# C-MPI-Scatter-Reduce-Parallel-Programming

This sample program utilizes Microsoft's MPI library, a message-passing system used for parallel programming.

## Geometric Mean

Several random numbers between 1-10 will be generated based on user input N.  The array of numbers will then be split among P processes using `MPI_Scatter`.  Each process will compute the product of the split and `MPI_Reduce` will be used to return the split products back to the root process 0.

Lastly, `pow(product, (1/N)` will be calculated to determine the Geometric Mean.

## Requirements

MPI SDK and Redistributable is required to compile on windows.  Please follow Microsof's guide to get going
https://blogs.technet.microsoft.com/windowshpc/2015/02/02/how-to-compile-and-run-a-simple-ms-mpi-program/

## Execution

Program can be executed after building using the following command 

`mpiexec -n P Geo-Mean.exe N` where P represents number of processes and <N> represents number of random numbers that will be generated

## Resources

This program is using Edd Mann's custom vector code for C that can be found under http://eddmann.com/posts/implementing-a-dynamic-vector-array-in-c/
