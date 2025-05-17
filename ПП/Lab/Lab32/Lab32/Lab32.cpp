#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    int N;

    if (rank == 0) {
        std::cout << "Enter N: ";
        std::cin >> N;
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    double h = 1.0 / N;
    double local_sum = 0.0;

#pragma omp parallel
    {
        double thread_sum = 0.0;

#pragma omp for
        for (int i = rank; i < N; i += size) {
            double x = (i + 0.5) * h;
            thread_sum += 4.0 / (1.0 + x * x);
        }

#pragma omp atomic
        local_sum += thread_sum;
    }

    double global_sum = 0.0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double pi = h * global_sum;
        std::cout << std::fixed << std::setprecision(15)
            << "Calculated pi = " << pi << std::endl;
    }

    MPI_Finalize();
    return 0;
}
