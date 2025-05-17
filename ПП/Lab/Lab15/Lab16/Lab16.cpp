#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); 

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    if (rank == 0) {
        std::cout << size << " processes." << std::endl;
    }

    if (rank % 2 == 0) {
        std::cout << "I am " << rank << ": FIRST!" << std::endl;
    }
    else {
        std::cout << "I am " << rank << ": SECOND!" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
