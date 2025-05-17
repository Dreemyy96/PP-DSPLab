#include <mpi.h>
#include <iostream>
#include <cmath>

double f(double x) {
    return 4.0 / (1.0 + x * x);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long N;
    if (rank == 0) {
        std::cout << "Enter N: ";
        std::cin >> N;
    }

    MPI_Bcast(&N, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    double h = 1.0 / N;  
    double local_sum = 0.0;

    for (long long i = rank; i < N; i += size) {
        double x = (i + 0.5) * h;
        local_sum += f(x);
    }

    local_sum *= h; 

    double global_sum = 0.0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout.precision(8);
        std::cout << "Result pi: " << global_sum << std::endl;
    }

    MPI_Finalize();
    return 0;
}
