#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>


// Função que verifica se um número é primo
int primo(long int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;

    long int i;
    for (i = 3; i*i <= n; i += 2) {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Valor inválido! Entre com o valor do maior inteiro\n");
        return 1;
    }

    long int n = strtol(argv[1], NULL, 10);
    int total = 0;

    clock_t inicio = clock();

    for (long int i = 3; i <= n; i += 2) {
        if (primo(i))
            total++;
    }

    total += 1; // contabiliza o número 2

    clock_t fim = clock();

    double tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC * 1000.0; // em milissegundos

    printf("Quant. de primos entre 1 e %ld: %d\n", n, total);
    printf("Tempo: %.2lf mseg\n", tempo);

    return 0;
}
