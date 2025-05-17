#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    const int tag = 0;

    if (rank == 0) {
        int received_message;
        for (int i = 1; i < size; i++) {
            MPI_Recv(&received_message, 1, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "receive message '" << received_message << "' from process " << i << std::endl;
        }
    }
    else {
        MPI_Send(&rank, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
