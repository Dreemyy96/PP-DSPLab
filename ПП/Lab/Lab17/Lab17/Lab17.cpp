#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        std::cerr << "Нужно как минимум 2 процесса!" << std::endl;
        MPI_Finalize();
        return 1;
    }

    const int tag = 0; 
    if (rank == 0) {
        const char message[] = "Hello from process 0!";
        MPI_Send(message, sizeof(message), MPI_CHAR, 1, tag, MPI_COMM_WORLD);
        std::cout << "Process 0 sent a message to Process 1." << std::endl;
    }
    else if (rank == 1) {
        char receivedMessage[50];
        MPI_Recv(receivedMessage, 50, MPI_CHAR, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process 1 received message: " << receivedMessage << std::endl;
    }

    MPI_Finalize(); 
    return 0;
}
