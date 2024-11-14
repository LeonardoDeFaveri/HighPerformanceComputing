#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>

struct Result {
    unsigned long size;
    double secs;
    double rate;
};

int main(int argc, char **argv) {
    const int test_count = 25;
    unsigned long bytes;

    int rank, world_size;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int tmp = 0;
    if (rank == 0) {
        // Send to initialize channel
        MPI_Send(&tmp, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&tmp, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        MPI_Recv(&tmp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&tmp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    struct Result results[test_count];
    for (int i = 0; i < test_count; i++) {
        bytes = 1 << i;
        __int8_t *array = malloc(sizeof(__int8_t) * bytes);
        double start, end;
        start = MPI_Wtime();
        if (rank == 0) {
            //MPI_Send(array, bytes, MPI_INT8_T, 1, 0, MPI_COMM_WORLD);
            //MPI_Recv(array, bytes, MPI_INT8_T, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(array, bytes, MPI_INT8_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(array, bytes, MPI_INT8_T, 0, 0, MPI_COMM_WORLD);
        }
        end = MPI_Wtime();
        free(array);
        struct Result result = (struct Result) {
            .size = bytes,
            .secs = (end - start) / 2,
            .rate = bytes / (1024 * 1024 * (end - start))
        };
        results[i] = result;
    }

    if (rank == 0) {
        printf("%-10s\t%-10s\t%-15s\n", "Size", "Time (sec)", "Rate (MB/sec)");
        for (int i = 0; i < test_count; i++) {
            struct Result res = results[i];
            printf("%-10lu\t%-10f\t%-15f\n", res.size, res.secs, res.rate);
        }
    }

    MPI_Finalize();
    return 0;
}