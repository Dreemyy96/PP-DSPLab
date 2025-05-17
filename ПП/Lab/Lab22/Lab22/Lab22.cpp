#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); 

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    const int tag = 0;
    int message = rank; 

    for (int i = 0; i < size; i++) {
        if (i != rank) {
            MPI_Send(&message, 1, MPI_INT, i, tag, MPI_COMM_WORLD); 
        }
    }

    for (int i = 0; i < size; i++) {
        if (i != rank) {
            int received_message;
            MPI_Recv(&received_message, 1, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
            std::cout << "[" << rank << "]: receive message '" << received_message << "' from " << i << std::endl;
        }
    }

    MPI_Finalize(); 
    return 0;
}
