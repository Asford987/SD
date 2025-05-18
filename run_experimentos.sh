#!/bin/bash

set -e
mkdir -p bin logs scripts

# ========= COMPILAÇÃO =========
echo "[Compilando programas...]"

gcc -O2 -o bin/seq_primos src/seq_primos.c /usr/lib64/libm.a
mpicc -O2 -o bin/mpi_saltos src/mpi_saltos.c /usr/lib64/libm.a
mpicc -O2 -o bin/mpi_saco src/mpi_saco.c /usr/lib64/libm.a

echo "[Compilação concluída.]"

# ========= EXECUÇÃO SEQUENCIAL =========
Ns=(100000000 1000000000)

echo "[Executando versão sequencial...]"
for N in "${Ns[@]}"; do
    ./bin/seq_primos $N > logs/seq_N${N}.log 2>&1
    echo "  [OK] seq N=$N"
done

# ========= TEMPLATE DE SUBMISSÃO QSUB =========
gerar_script_qsub() {
    versao=$1
    np=$2
    N=$3
    script="scripts/${versao}_N${N}_np${np}.qsub"

    cat << EOF > $script
#!/bin/bash
#PBS -l nodes=1:ppn=${np}
#PBS -N ${versao}_N${N}_np${np}
#PBS -o logs/${versao}_N${N}_np${np}.log
#PBS -e logs/${versao}_N${N}_np${np}.err
#PBS -q batch

cd \$PBS_O_WORKDIR
mpirun -np ${np} ./bin/${versao} ${N}
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
