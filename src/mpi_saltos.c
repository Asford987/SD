#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int primo(long int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;

    long int i;
    for (i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int rank, size;
    long int N;
    int local_total = 0, total = 0;
    double inicio_tempo, fim_tempo;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0)
            printf("Entre com N.\n");
        MPI_Finalize();
        return 1;
    }

    N = strtol(argv[1], NULL, 10);
    long int i;

    inicio_tempo = MPI_Wtime();

    for (i = 3 + rank * 2; i <= N; i += size * 2) {
        if (primo(i)) local_total++;
    }

    if (rank == 0) local_total += 1; // incluir o 2

    MPI_Reduce(&local_total, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    fim_tempo = MPI_Wtime();

    if (rank == 0) {
        printf("Quant. de primos entre 1 e %ld: %d\n", N, total);
        printf("Tempo: %.2lf segundos\n", fim_tempo - inicio_tempo);
    }

    MPI_Finalize();
    return 0;
}
