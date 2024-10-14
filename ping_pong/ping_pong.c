#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    // Number of times each process must perform a send
    unsigned int count = 10;
    if (argc > 1) {
        count = atoi(argv[1]);
    }

    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int value;
    int other;
    // If there's an odd number of processes, the last one does nothing
    if (world_size % 2 == 1 && rank == world_size - 1) {
        return 0;
    }

    if (rank % 2 == 0) {
        value = 0;
        // Each process sends data to the process with successive rank
        other = rank + 1;
        MPI_Send(&value, 1, MPI_INT, other, 0, MPI_COMM_WORLD);
        printf("[P%d] Send %d\n", rank, value);
    } else {
        // Each process expects to receive data from process with predecessor rank
        other = rank - 1;
    }

    for (int i = 0; i < count - 1; i++) {
        MPI_Recv(&value, 1, MPI_INT, other, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("[P%d] Recv %d\n", rank, value);
        value++;
        printf("[P%d] Send %d\n", rank, value);
        MPI_Send(&value, 1, MPI_INT, other, 0, MPI_COMM_WORLD);
    }

    if (rank % 2 == 1) {
        MPI_Recv(&value, 1, MPI_INT, other, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("[P%d] Recv %d\n", rank, value);
    }

    MPI_Finalize();
    return 0;
}