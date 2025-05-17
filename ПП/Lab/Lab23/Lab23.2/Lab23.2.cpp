#include <mpi.h>
#include <iostream>
#include <vector>
#include <map>

void count_characters(const std::string& str, std::map<char, int>& freq) {
    for (char c : str) {
        freq[c]++;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;
    char str[101];

    if (rank == 0) {
        std::cout << "Enter string length: ";
        std::cin >> n;
        std::cout << "Enter string: ";
        std::cin >> str;

        for (int i = 1; i < size; i++) {
            MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(str, n, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(str, n, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        str[n] = '\0'; 
    }

    int chunk_size = n / size;
    int start = rank * chunk_size;
    int end = (rank == size - 1) ? n : start + chunk_size;

    std::map<char, int> local_freq;
    count_characters(std::string(str + start, str + end), local_freq);

    if (rank != 0) {
        for (char c = 'a'; c <= 'z'; ++c) {
            int count = local_freq[c];
            MPI_Send(&count, 1, MPI_INT, 0, c, MPI_COMM_WORLD);
        }
    }
    else {
        std::map<char, int> global_freq = local_freq;
        for (int i = 1; i < size; i++) {
            for (char c = 'a'; c <= 'z'; ++c) {
                int count;
                MPI_Recv(&count, 1, MPI_INT, i, c, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                global_freq[c] += count;
            }
        }

        for (auto& [key, value] : global_freq) {
            if (value > 0)
                std::cout << key << " = " << value << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
