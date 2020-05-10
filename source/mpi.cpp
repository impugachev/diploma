#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    
    int processorNumber;
    MPI_Comm_rank(MPI_COMM_WORLD, &processorNumber);
    if (processorNumber == 0)
    {
        printf("I'm master processor\n");
    }
    else
    {
        printf("I'm %d processor", processorNumber);
    }
    MPI_Finalize();

    return 0;
}