#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TAMANHO_TAREFA 500000
#define TAG_TAREFA 1
#define TAG_RESULTADO 2
#define TAG_FINALIZA 3

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

int conta_primos(long int inicio, long int fim) {
    int total = 0;
    for (long int i = inicio; i <= fim; i += 2) {
        if (primo(i)) total++;
    }
    return total;
}

int main(int argc, char *argv[]) {
    int rank, size;
    long int N;
    double inicio_tempo, fim_tempo;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // ---------- MESTRE ----------
        if (argc < 2) {
            printf("Entre com N.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        N = strtol(argv[1], NULL, 10);
        long int atual = 3;
        int total_primos = 1; // começa com 2
        int trabalhando = size - 1;
        MPI_Status status;

        inicio_tempo = MPI_Wtime();

        // Enviar tarefas iniciais
        for (int i = 1; i < size && atual <= N; i++) {
            long int tarefa[2];
            tarefa[0] = atual;
            tarefa[1] = atual + 2 * (TAMANHO_TAREFA - 1);
            if (tarefa[1] > N) tarefa[1] = N;
            MPI_Send(tarefa, 2, MPI_LONG, i, TAG_TAREFA, MPI_COMM_WORLD);
            atual = tarefa[1] + 2;
        }

        // Loop principal de recebimento/envio
        while (trabalhando > 0) {
            int local_total;
            MPI_Recv(&local_total, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RESULTADO, MPI_COMM_WORLD, &status);
            total_primos += local_total;

            if (atual <= N) {
                long int tarefa[2];
                tarefa[0] = atual;
                tarefa[1] = atual + 2 * (TAMANHO_TAREFA - 1);
                if (tarefa[1] > N) tarefa[1] = N;
                MPI_Send(tarefa, 2, MPI_LONG, status.MPI_SOURCE, TAG_TAREFA, MPI_COMM_WORLD);
                atual = tarefa[1] + 2;
            } else {
                MPI_Send(NULL, 0, MPI_LONG, status.MPI_SOURCE, TAG_FINALIZA, MPI_COMM_WORLD);
                trabalhando--;
            }
        }

        fim_tempo = MPI_Wtime();
        printf("Quant. de primos entre 1 e %ld: %d\n", N, total_primos);
        printf("Tempo: %.2lf segundos\n", fim_tempo - inicio_tempo);

    } else {
        // ---------- TRABALHADOR ----------
        MPI_Status status;
        while (1) {
            long int tarefa[2];
            MPI_Recv(tarefa, 2, MPI_LONG, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == TAG_FINALIZA) {
                break;
            }

            int local_total = conta_primos(tarefa[0], tarefa[1]);
            MPI_Send(&local_total, 1, MPI_INT, 0, TAG_RESULTADO, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
