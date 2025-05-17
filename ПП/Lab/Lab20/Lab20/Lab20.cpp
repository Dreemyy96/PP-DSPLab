#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int tag = 0;
    int message = 42; 

    if (rank == 0) {
        MPI_Request send_request;
        MPI_Isend(&message, 1, MPI_INT, 1, tag, MPI_COMM_WORLD, &send_request);
        MPI_Wait(&send_request, MPI_STATUS_IGNORE); 
    }
    else if (rank == 1) {
        int received_message;
        MPI_Request recv_request;
        MPI_Irecv(&received_message, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &recv_request);
        MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
        std::cout << "Process " << rank << " received message: " << received_message << std::endl;
    }

    MPI_Finalize();
    return 0;
}
