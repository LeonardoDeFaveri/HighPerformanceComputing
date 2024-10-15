#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>

struct BroadcastMessage {
    int value;
    int world_size;
};
typedef struct BroadcastMessage BroadcastMessage;

MPI_Datatype register_message_struct() {
    const int n_fields = 2;
    int block_lengths[] = {1, 1};
    MPI_Datatype field_types[] = {MPI_INT, MPI_INT};
    MPI_Datatype mpi_message;
    MPI_Aint offset[n_fields];

    offset[0] = offsetof(BroadcastMessage, value);
    offset[1] = offsetof(BroadcastMessage, world_size);

    MPI_Type_create_struct(n_fields, block_lengths, offset, field_types, &mpi_message);
    MPI_Type_commit(&mpi_message);
    return mpi_message;
}

int main(int argc, char **argv) {
    int rank, world_size;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Datatype mpi_message = register_message_struct();

    // Reduction phase
    int value;
    if (rank == 0) {
        value = 0;
        for (int i = 1; i < world_size; i++) {
            int recived_value;
            MPI_Status status;
            MPI_Recv(&recived_value, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            printf("[P%d] Recv %d from %d\n", rank, recived_value, status.MPI_SOURCE);
            value += recived_value;
        }
        printf("[P%d] Final Value: %d\n", rank, value);
    } else {
        srand(time(NULL));
        int value = rand() % world_size;
        printf("[P%d] Value: %d\n", rank, value);
        MPI_Send(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Broadcast phase: tree structure
    BroadcastMessage msg;
    if (rank == 0) {
        do {
            world_size /= 2;
            msg = (BroadcastMessage) {
                .value = value,
                .world_size = world_size
            };
            MPI_Send(&msg, 1, mpi_message, msg.world_size, 0, MPI_COMM_WORLD);
        } while (world_size > 1);
    } else {
        MPI_Status status;
        MPI_Recv(&msg, 1, mpi_message, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        printf("[P%d] Recv %d from %d\n", rank, msg.value, status.MPI_SOURCE);
        while (msg.world_size > 1) {
            msg.world_size /= 2;
            MPI_Send(&msg, 1, mpi_message, rank + msg.world_size, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Type_free(&mpi_message);
    MPI_Finalize();

    return 0;
}