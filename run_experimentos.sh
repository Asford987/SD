#!/bin/bash

set -e
mkdir -p bin logs scripts

# ========= COMPILAÇÃO =========
echo "[Compilando programas...]"

gcc -std=c99 -O2 -o bin/seq_primos src/seq_primos.c /usr/lib64/libm.a
mpicc -std=c99 -O2 -o bin/mpi_saltos src/mpi_saltos.c /usr/lib64/libm.a
mpicc -std=c99 -O2 -o bin/mpi_saco src/mpi_saco.c /usr/lib64/libm.a

echo "[Compilação concluída.]"

# ========= EXECUÇÃO SEQUENCIAL =========
Ns=(100000000 1000000000)

echo "[Executando versão sequencial...]"
for N in "${Ns[@]}"; do
    output_file="logs/seq_primos_N${N}.txt"
    ./bin/seq_primos $N > "$output_file" 2> "logs/seq_primos_N${N}.err"
    echo "  [OK] seq N=$N -> $output_file"
done

# ========= TEMPLATE DE SUBMISSÃO QSUB =========
gerar_script_qsub() {
    versao=$1
    np=$2
    N=$3
    script="scripts/${versao}_N${N}_np${np}.qsub"
    output_file="logs/${versao}_N${N}_np${np}.txt"
    error_file="logs/${versao}_N${N}_np${np}.err"

    cat << EOF > $script
#!/bin/bash
#PBS -l nodes=1:ppn=${np}
#PBS -N ${versao}_N${N}_np${np}
#PBS -o ${output_file}
#PBS -e ${error_file}
#PBS -q batch

cd \$PBS_O_WORKDIR
mpirun -np ${np} ./bin/${versao} ${N} > ${output_file} 2>> ${error_file}
EOF

    chmod +x $script
    qsub $script
}

# ========= EXECUÇÃO MPI =========
versoes=("mpi_saltos" "mpi_saco")

for versao in "${versoes[@]}"; do
    for N in "${Ns[@]}"; do
        for np in {2..16}; do
            gerar_script_qsub $versao $np $N
            echo "  [Enviado] $versao | N=$N | np=$np"
        done
    done
done

echo "[Todas as tarefas foram submetidas.]"
