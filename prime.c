#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int prime_test (int N, int rank, int size)
{
    int h = N / 2;
    int k = h / size + 1;
    int lower = rank * k + 2;
    int upper = (rank + 1) * k + 1;

    if ( upper > h )
        upper = h;

    if ( rank == 0 )
        lower = 2;

    int i;

    printf ("From %d: [%d, %d]\n", rank, lower, upper);

    for (i=lower; i<=upper; i++) {
        if ( N % i == 0 )
            return 1;
    }

    return 0;
}

int main (int argc, char** argv)
{
    int myid, numprocs, processor_name_length;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init (0, 0);
    MPI_Comm_rank (MPI_COMM_WORLD, &myid);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Get_processor_name (processor_name, &processor_name_length);

    int N, i, result = 0;

    // Input N on master
    if (myid == 0) {
        printf ("Input N: ");
        scanf ("%d", &N);
    }

    if (myid == 0) {
        // Send number to all slaves
        for(i=1; i<numprocs; i++)
	    MPI_Send(&N, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

        // Calculate on master for first N/2/numprocs numbers
        result = prime_test (N, myid, numprocs);

        // Get results from slaves
        int result_slave = 0;
        for(i=1; i<numprocs; i++) {
	    MPI_Recv(&result_slave, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            result += result_slave;
        }

        // Print message
        if (result == 0) {
            printf("Number is prime!\n");
        } else {
            printf("Number is not prime!\n");
        }
    } else {
        // Recieve number from master
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // test for prime
        result = prime_test (N, myid, numprocs);

        // send result to master
        MPI_Send(&result, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

