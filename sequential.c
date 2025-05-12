#include <stdio.h>
#include <stdlib.h>

#define N 1000000
#define NUM_PARTS 10

int main() {
    int *vetor = malloc(N * sizeof(int));
    int soma_total = 0;
    int somas_parciais[NUM_PARTS] = {0};
    int maior_soma = 0;
    int indice_maior_soma = 0;

    // Inicialização do vetor
    for (int i = 0; i < N; i++) {
        if (i % 2 == 0)
            vetor[i] = (i * 2) % 10;
        else
            vetor[i] = i % 10;
    }

    // Tamanho de cada parte simulada
    int local_n = N / NUM_PARTS;

    // Cálculo das somas parciais
    for (int part = 0; part < NUM_PARTS; part++) {
        int soma = 0;
        for (int i = part * local_n; i < (part + 1) * local_n; i++) {
            soma += vetor[i];
        }
        somas_parciais[part] = soma;
        soma_total += soma;

        if (soma > maior_soma) {
            maior_soma = soma;
            indice_maior_soma = part;
        }
    }

    printf("Soma total dos elementos do vetor: %d\n", soma_total);
    printf("Simulado processo com maior soma parcial: %d (soma: %d)\n", indice_maior_soma, maior_soma);

    free(vetor);
    return 0;
}
