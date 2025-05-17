#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); 

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    const int tag = 0;
    int message = rank; 

    int next = (rank + 1) % size;
    int prev = (rank - 1 + size) % size; 

    int received_message;
    MPI_Sendrecv(&message, 1, MPI_INT, next, tag,
        &received_message, 1, MPI_INT, prev, tag,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    std::cout << "[" << rank << "]: receive message '" << received_message << "'" << std::endl;

    MPI_Finalize(); 
    return 0;
}
