#!/bin/bash

# Limpa o relatório anterior, se existir
relatorio="relatorio_experimentos.txt"
> "$relatorio"

# Estrutura de dados temporária em um arquivo auxiliar
declare -A dados

# Loop pelos arquivos que seguem o padrão esperado
for arquivo in mpi_*.txt; do
    # Ignora se não for arquivo comum
    [[ -f "$arquivo" ]] || continue

    # Extrai informações do nome do arquivo usando regex
    if [[ "$arquivo" =~ ^(mpi_saco|mpi_saltos)_N(100M|1B)_np([0-9]{1,2})\.txt$ ]]; then
        algoritmo="${BASH_REMATCH[1]}"
        N="${BASH_REMATCH[2]}"
        np="${BASH_REMATCH[3]}"

        # Verifica se np está no intervalo desejado
        if ((np < 2 || np > 16)); then
            continue
        fi

        # Extrai a quantidade de primos e o tempo
        quantidade=$(grep "Quant." "$arquivo" | grep -oE '[0-9]+$')
        tempo=$(grep "Tempo" "$arquivo" | grep -oE '[0-9]+(\.[0-9]+)?')

        # Salva os dados numa variável associativa indexada
        chave="${algoritmo}_${N}_${np}"
        dados["$chave"]="$quantidade|$tempo"
    fi
done

# Gera o relatório formatado
for algoritmo in mpi_saco mpi_saltos; do
    echo "$algoritmo:" >> "$relatorio"
    for N in 100M 1B; do
        echo "    $N:" >> "$relatorio"
        echo "        processadores:" >> "$relatorio"
        for np in {2..16}; do
            chave="${algoritmo}_${N}_${np}"
            if [[ -n "${dados[$chave]}" ]]; then
                IFS="|" read quantidade tempo <<< "${dados[$chave]}"
                echo "            $np:" >> "$relatorio"
                echo "                quantidade: $quantidade" >> "$relatorio"
                echo "                tempo: $tempo segundos" >> "$relatorio"
            fi
        done
    done
done

echo "[Relatório gerado em '$relatorio']"
