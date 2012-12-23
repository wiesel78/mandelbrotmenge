#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int size, rank, i, r;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if(rank != 0)
    {
        MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    
    if(rank == 0)
    {
        for(i = 1 ; i < size ; i++)
        {
            MPI_Recv(&r, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            
            printf("ich bin prozess %d von %d\n",  status.MPI_SOURCE, size);
        }
    }
    
    MPI_Finalize();

    return 0;
}
