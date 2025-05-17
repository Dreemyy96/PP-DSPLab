#include <mpi.h>
#include <iostream>
#include <vector>
using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int color = (world_rank % 2 == 0) ? 1 : MPI_UNDEFINED;
    MPI_Comm new_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &new_comm);

    int new_rank = -1, new_size = -1;
    string message;

    if (color == 1) {
        MPI_Comm_rank(new_comm, &new_rank);
        MPI_Comm_size(new_comm, &new_size);

        if (new_rank == 0) {
            cin >> message;
        }

        char msg_buf[11] = { 0 };
        if (new_rank == 0) {
            strncpy_s(msg_buf, message.c_str(), 10);
        }

        MPI_Bcast(msg_buf, 10, MPI_CHAR, 0, new_comm);
        message = msg_buf;
    }
    else {
        message = "no";
    }

    if (color == 1) {
        cout << "MPI_COMM_WORLD: " << world_rank << " from " << world_size
            << ". New comm: " << new_rank << " from " << new_size
            << ". Message = " << message << endl;
    }
    else {
        cout << "MPI_COMM_WORLD: " << world_rank << " from " << world_size
            << ". New comm: no from no. Message = no" << endl;
    }

    if (color == 1) {
        MPI_Comm_free(&new_comm);
    }

    MPI_Finalize();
    return 0;
}
