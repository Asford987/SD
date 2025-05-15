#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000000

int main(int argc, char *argv[]) {
    int rank, size;
    int *vetor = NULL;
    int local_n;
    int *local_vetor;
    int local_sum = 0, total_sum = 0;
    struct {
        int value;
        int rank;
    } local_max, global_max;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 10) {
        if (rank == 0) {
            fprintf(stderr, "Este programa precisa ser executado com 10 processos.\n");
        }
        MPI_Finalize();
        return 1;
    }

    local_n = N / size;

    if (rank == 0) {
        vetor = malloc(N * sizeof(int));
        for (int i = 0; i < N; i++) {
            if (i % 2 == 0)
                vetor[i] = (i * 2) % 10;
            else
                vetor[i] = i % 10;
        }
    }

    local_vetor = malloc(local_n * sizeof(int));

    MPI_Scatter(vetor, local_n, MPI_INT, local_vetor, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_n; i++) {
        local_sum += local_vetor[i];
    }

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    local_max.value = local_sum;
    local_max.rank = rank;

    MPI_Reduce(&local_max, &global_max, 1, MPI_2INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Soma total dos elementos do vetor: %d\n", total_sum);
        printf("Processo com maior soma parcial: %d (soma: %d)\n", global_max.rank, global_max.value);
    }

    free(local_vetor);
    if (rank == 0) free(vetor);

    MPI_Finalize();
    return 0;
}
