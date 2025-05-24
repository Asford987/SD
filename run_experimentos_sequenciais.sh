#!/bin/bash

set -e
mkdir -p bin logs scripts

# ========= COMPILAÇÃO =========
echo "[Compilando programas...]"

gcc -std=c99 -O2 -o bin/seq_primos src/seq_primos.c

echo "[Compilação concluída.]"

# ========= EXECUÇÃO SEQUENCIAL =========
Ns=(100000000 1000000000)

echo "[Executando versão sequencial...]"
for N in "${Ns[@]}"; do
    output_file="logs/seq_primos_N${N}.txt"
    ./bin/seq_primos $N > "$output_file" 2> "logs/seq_primos_N${N}.err"
    echo "  [OK] seq N=$N -> $output_file"
done
