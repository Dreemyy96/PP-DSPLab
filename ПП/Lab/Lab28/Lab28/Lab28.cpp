#include <mpi.h>
#include <iostream>
#include <cmath>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N;
    double step, local_sum = 0.0;

    if (rank == 0) {
        std::cin >> N;
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    step = 1.0 / static_cast<double>(N);

    for (int i = rank; i < N; i += size) {
        double x = (i + 0.5) * step;
        local_sum += 1.0 / (1.0 + x * x);
    }

    double* result_buffer = nullptr;
    if (rank == 0) {
        result_buffer = new double[size]();
    }

    MPI_Win win;
    MPI_Win_create(result_buffer, (rank == 0 ? sizeof(double) * size : 0), sizeof(double),
        MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    MPI_Win_fence(0, win);

    MPI_Put(&local_sum, 1, MPI_DOUBLE, 0, rank, 1, MPI_DOUBLE, win);

    MPI_Win_fence(0, win);

    if (rank == 0) {
        double total_sum = 0.0;
        for (int i = 0; i < size; ++i) {
            total_sum += result_buffer[i];
        }

        double pi = total_sum * step * 4.0;
        std::cout.precision(10);
        std::cout << pi << std::endl;

        delete[] result_buffer;
    }

    MPI_Win_free(&win);
    MPI_Finalize();
    return 0;
}
