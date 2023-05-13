#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Sender and reciever id
    int sender = 0;
    int receiver = 1;

    // Size of message in bytes
    int message_size = 1024;

    // Message buffer
    char* buffer = new char[message_size];

    if (world_rank == sender) {
        // Sending
        double start_time = MPI_Wtime();
        MPI_Send(buffer, message_size, MPI_CHAR, receiver, 0, MPI_COMM_WORLD);
        double end_time = MPI_Wtime();
        std::cout << "Message sended in " << end_time - start_time << " seconds\n";
    }
    else if (world_rank == receiver) {
        // Getting message
        char* recvbuf = new char[message_size];
        MPI_Status status;
        MPI_Recv(recvbuf, message_size, MPI_CHAR, sender, 0, MPI_COMM_WORLD, &status);
    }

    // Cleaning buffer
    delete[] buffer;
    // MPI process closing
    MPI_Finalize();

    return 0;
}
