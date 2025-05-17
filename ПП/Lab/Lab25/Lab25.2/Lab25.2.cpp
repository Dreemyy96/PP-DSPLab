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
    vector<double> B;

    if (rank == 0) {
        cin >> n;
        vector<double> A(n * n), C(n * n);
        B.resize(n * n);

        for (int i = 0; i < n * n; i++) cin >> A[i];
        for (int i = 0; i < n * n; i++) cin >> B[i];

        for (int i = 1; i < size; i++) {
            MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(B.data(), n * n, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
        }

        for (int i = 1; i < size; i++) {
            MPI_Send(A.data() + i * n, n, MPI_DOUBLE, i, 2, MPI_COMM_WORLD);
        }

        for (int j = 0; j < n; j++) {
            multiply_row_by_matrix(vector<double>(A.begin() + j * n, A.begin() + (j + 1) * n), B, C, n);
        }

        for (int i = 1; i < size; i++) {
            MPI_Recv(C.data() + i * n, n, MPI_DOUBLE, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << C[i * n + j] << " ";
            }
            cout << endl;
        }
    }
    else {
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        B.resize(n * n);
        MPI_Recv(B.data(), n * n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        vector<double> local_row(n), local_result(n);
        MPI_Recv(local_row.data(), n, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        multiply_row_by_matrix(local_row, B, local_result, n);

        MPI_Send(local_result.data(), n, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
