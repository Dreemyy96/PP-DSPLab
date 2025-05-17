#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    MPI_Comm parent_comm;
    MPI_Comm_get_parent(&parent_comm);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (parent_comm == MPI_COMM_NULL) {
        std::cout << "I am " << world_rank << " process from "
            << world_size << " processes!\n" << "My parent is none." << std::endl;
            
        if (world_rank == 0) {
            int n;
            std::cin >> n;

            MPI_Comm child_comm;
            MPI_Info info;
            MPI_Info_create(&info);

            MPI_Comm_spawn("Lab27.exe", MPI_ARGV_NULL, n,
                info, 0, MPI_COMM_SELF, &child_comm,
                MPI_ERRCODES_IGNORE);

            for (int i = 0; i < n; ++i) {
                int parent_rank = world_rank;
                MPI_Send(&parent_rank, 1, MPI_INT, i, 0, child_comm);
            }
        }

    }
    else {
        int parent_rank;
        MPI_Recv(&parent_rank, 1, MPI_INT, 0, 0, parent_comm, MPI_STATUS_IGNORE);

        std::cout << "I am " << world_rank << " process from "
            << world_size << " processes!\n" << "My parent is " << parent_rank << "." << std::endl;
            
    }

    MPI_Finalize();
    return 0;
}
