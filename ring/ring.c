#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

struct Message {
    int value;
    int original_sender_rank;
};

MPI_Datatype register_message_struct() {
    const int n_fields = 2;
    int block_lengths[] = {1, 1};
    MPI_Datatype field_types[] = {MPI_INT, MPI_INT};
    MPI_Datatype mpi_message;
    MPI_Aint offset[n_fields];

    offset[0] = offsetof(struct Message, value);
    offset[1] = offsetof(struct Message, original_sender_rank);

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

    struct Message msg;
    if (rank == 0) {
        msg = (struct Message) {
            .value = 0,
            .original_sender_rank = rank
        };
        MPI_Send(&msg, 1, mpi_message, rank + 1, 0, MPI_COMM_WORLD);
        printf(
            "[P%d] Send: {value = %d, original_sender_rank = %d}\n",
            rank, msg.value, msg.original_sender_rank
        );
        MPI_Recv(&msg, 1, mpi_message, world_size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf(
            "[P%d] Recv: {value = %d, original_sender_rank = %d}\n",
            rank, msg.value, msg.original_sender_rank
        );
    } else {
        MPI_Recv(&msg, 1, mpi_message, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf(
            "[P%d] Recv: {value = %d, original_sender_rank = %d}\n",
            rank, msg.value, msg.original_sender_rank
        );
        msg.value++;
        MPI_Send(&msg, 1, mpi_message, (rank + 1) % world_size, 0, MPI_COMM_WORLD);
        printf(
            "[P%d] Send: {value = %d, original_sender_rank = %d}\n",
            rank, msg.value, msg.original_sender_rank
        );
    }

    MPI_Type_free(&mpi_message);
    MPI_Finalize();

    return 0;
}