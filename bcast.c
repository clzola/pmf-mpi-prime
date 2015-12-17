#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main (int argc, char** argv)
{
    int myid, numprocs, processor_name_length;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init (0, 0);
    MPI_Comm_rank (MPI_COMM_WORLD, &myid);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Get_processor_name (processor_name, &processor_name_length);

    int number = 0;
    if (myid == 0) {
        printf ("Input number: ");
        scanf ("%d", &number);
    }

    MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf ("Hello from process %d of %d. Number: %d [%s]\n",
                   myid, numprocs, number, processor_name);

    MPI_Finalize ();
    return 0;
}

