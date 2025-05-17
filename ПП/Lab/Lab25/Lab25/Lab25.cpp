#include <mpi.h>
#include <iostream>
#include <vector>

using namespace std;

void multiply_row_by_matrix(const vector<double>& row, const vector<double>& B, vector<double>& result, int n) {
    for (int j = 0; j < n; j++) {
        result[j] = 0;
        for (int k = 0; k < n; k++) {
            result[j] += row[k] * B[k * n + j];
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;
    vector<double> A, B, C;

    if (rank == 0) {
        cin >> n;
        A.resize(n * n);
        B.resize(n * n);
        C.resize(n * n);

        for (int i = 0; i < n * n; i++) cin >> A[i];
        for (int i = 0; i < n * n; i++) cin >> B[i];
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    B.resize(n * n);
    MPI_Bcast(B.data(), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    vector<double> local_row(n), local_result(n);
    MPI_Scatter(A.data(), n, MPI_DOUBLE, local_row.data(), n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    multiply_row_by_matrix(local_row, B, local_result, n);

    MPI_Gather(local_result.data(), n, MPI_DOUBLE, C.data(), n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << C[i * n + j] << " ";
            }
            cout << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
