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
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(str, n, MPI_CHAR, 0, MPI_COMM_WORLD);
    str[n] = '\0';

    int chunk_size = n / size;
    int start = rank * chunk_size;
    int end = (rank == size - 1) ? n : start + chunk_size;

    std::map<char, int> local_freq;
    count_characters(std::string(str + start, str + end), local_freq);

    std::map<char, int> global_freq;
    for (char c = 'a'; c <= 'z'; ++c) {
        int local_count = local_freq[c];
        int global_count;
        MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if (rank == 0 && global_count > 0) {
            std::cout << c << " = " << global_count << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
