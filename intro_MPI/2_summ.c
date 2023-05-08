#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void summator(int rank, unsigned int N, int commsize)
{
    long double step = 0.0;

    unsigned int work_amount = N / commsize;
    unsigned int start = work_amount * (rank - 1) + 1;
    if (rank == commsize) {
        work_amount += N % commsize;
    }
    for (unsigned int i = start, end = start + work_amount; i < end; ++i) {
        step += 1.0 / i;
    }

    // int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
    MPI_Send(&step, 1, MPI_LONG_DOUBLE, 0, 0, MPI_COMM_WORLD);
}

long double finalize(unsigned int commsize)
{
    long double result = 0.0;
    for (unsigned int i = 1; i < commsize; ++i)
    {
        long double i_res = 0.0;
        MPI_Status status;

        // int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status
        MPI_Recv(&i_res, 1, MPI_LONG_DOUBLE, i, 0, MPI_COMM_WORLD, &status);

        result += i_res;
    }

    return result;
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the number of processes
    int commsize;
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);

    if (argc >= 2)
    {
        if (rank == 0) {
            printf("sum = %Lf\n", finalize(commsize));
        } else {
            summator(rank, atoi(argv[1]), commsize - 1);
        }
    } else if (rank == 0) {
        printf("USAGE: %s AMOUNT_OF_SUM_ELEMS\n", argv[0]);
    }

    MPI_Finalize();
}