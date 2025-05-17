#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); 

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int message;
    const int tag = 0;

    if (rank == 0) {
        message = 0;
        MPI_Send(&message, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
        MPI_Recv(&message, 1, MPI_INT, size - 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "[0]: receive message '" << message << "'" << std::endl;
    }
    else {
        MPI_Recv(&message, 1, MPI_INT, rank - 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "[" << rank << "]: receive message '" << message << "'" << std::endl;

        message++; 

        int next = (rank + 1) % size;
        MPI_Send(&message, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
