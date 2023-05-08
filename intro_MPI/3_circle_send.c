#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int task(unsigned int message) {
    return (message += 1);
}

void print_status(unsigned int message, int rank) {
    printf("----------------------\n");
    printf("Rank          = %d\n", rank);
    printf("Curr message  = %d\n", message);
}

void do_zero(unsigned int message, unsigned int commsize) {

    print_status(message, 0);
    unsigned int dst = 1;
    unsigned int from = commsize - 1;
    if (commsize == 1) {
        dst = from = 0;
    }
    MPI_Status status;
    // int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status
    MPI_Send(&message, 1, MPI_UNSIGNED, dst, 0, MPI_COMM_WORLD);
    MPI_Recv(&message, 1, MPI_UNSIGNED, from, 0, MPI_COMM_WORLD, &status);
    print_status(task(message), 0);
}

void circle_send(unsigned int message, int rank, unsigned int commsize) {

    if (rank == 0) {
        do_zero(message, commsize);
        return;
    }

    unsigned int from_rank = rank - 1;
    unsigned int to_rank = (unsigned int)rank == commsize - 1 ? 0 : rank + 1;
    unsigned int cur_message = 0;
    MPI_Status status;
    MPI_Recv(&cur_message, 1, MPI_UNSIGNED, from_rank, 0, MPI_COMM_WORLD, &status);
    print_status(++cur_message, rank);
    // int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
    MPI_Send(&cur_message, 1, MPI_UNSIGNED, to_rank, 0, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);
    setbuf(stdout, NULL);
    
    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the number of processes
    int commsize;
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);

    circle_send(0, rank, commsize);

    MPI_Finalize();
}