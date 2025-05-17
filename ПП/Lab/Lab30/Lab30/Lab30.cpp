#include <iostream>
#include <mpi.h>
#include <cmath>

int main(int argc, char** argv) {
    int rank, size;
    long long N;

    MPI_Init(&argc, &argv);                     
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);       
    MPI_Comm_size(MPI_COMM_WORLD, &size);       

    if (rank == 0) {
        std::cin >> N; 
    }

    MPI_Bcast(&N, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime(); 

    double step = 1.0 / static_cast<double>(N);
    double local_sum = 0.0;

    for (long long i = rank; i < N; i += size) {
        double x = (i + 0.5) * step;
        local_sum += 4.0 / (1.0 + x * x);
    }

    double global_sum = 0.0;

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime(); 

    if (rank == 0) {
        double pi = step * global_sum;
        std::cout.precision(15);
        std::cout << pi << std::endl;
        std::cout << "Execution time: " << (end_time - start_time) << " seconds" << std::endl;
    }

    MPI_Finalize(); 
    return 0;
}
