#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int prime_test (int N, int rank, int size, char* name)
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

    printf ("From %d: [%d, %d] (node: %s)\n", rank, lower, upper, name);

    for (i=lower; i<=upper; i++) {
        if ( N % i == 0 )
            return 1;
    }

    return 0;
}

int main (int argc, char** argv)
{
    int myid, numprocs, len;
    char node_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init (0, 0);
    MPI_Comm_rank (MPI_COMM_WORLD, &myid);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Get_processor_name(node_name, &len);

    int N, i, result = 0;

    // Input N on master
    if (myid == 0) {
        printf ("Input N: ");
        scanf ("%d", &N);
    }

    MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (myid == 0) {
        // Calculate on master for first N/2/numprocs numbers
        result = prime_test (N, myid, numprocs, node_name);

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
        // Number N is recieved via boradcast
        // test for prime on slave
        result = prime_test (N, myid, numprocs, node_name);

        // send result to master
        MPI_Send(&result, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

