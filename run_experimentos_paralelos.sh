#!/bin/bash

set -e
mkdir -p bin logs scripts

# ========= COMPILAÇÃO =========
echo "[Compilando programas...]"

mpicc -std=c99 -O2 -o bin/mpi_saltos src/mpi_saltos.c
mpicc -std=c99 -O2 -o bin/mpi_saco src/mpi_saco.c

echo "[Compilação concluída.]"

Ns=(100000000 1000000000)

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
#$ -cwd
#$ -j y
#$ -S /bin/bash
#$ -N ${versao}_N${N}_np${np}
#$ -pe mpi ${np}
#$ -o ${output_file}
#$ -e ${error_file}

cd \$PWD
/opt/openmpi/bin/mpirun -np ${np} ./bin/${versao} ${N}
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
