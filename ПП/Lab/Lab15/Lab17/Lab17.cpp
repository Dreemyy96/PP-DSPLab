#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Получаем номер текущего процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Получаем общее количество процессов

    if (size < 2) {
        std::cerr << "Нужно как минимум 2 процесса!" << std::endl;
        MPI_Finalize();
        return 1;
    }

    const int tag = 0; // Метка сообщения
    if (rank == 0) {
        // Процесс 0 отправляет сообщение процессу 1
        const char message[] = "Hello from process 0!";
        MPI_Send(message, sizeof(message), MPI_CHAR, 1, tag, MPI_COMM_WORLD);
        std::cout << "Process 0 sent a message to Process 1." << std::endl;
    }
    else if (rank == 1) {
        // Процесс 1 принимает сообщение от процесса 0
        char receivedMessage[50]; // Буфер для получения сообщения
        MPI_Recv(receivedMessage, 50, MPI_CHAR, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process 1 received message: " << receivedMessage << std::endl;
    }

    MPI_Finalize(); // Завершаем работу MPI
    return 0;
}
