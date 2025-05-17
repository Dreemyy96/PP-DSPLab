#include <mpi.h>
#include <omp.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); 

    int n;
    if (world_rank == 0) {
        std::cout << "Enter number of threads: ";
        std::cin >> n;
    }


    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    omp_set_num_threads(n);

    #pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        int total_hybrid_threads = n * world_size;

        #pragma omp critical
        {
            std::cout << "I am " << thread_num
                << " thread from " << world_rank
                << " process. Number of hybrid threads = "
                << total_hybrid_threads << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
