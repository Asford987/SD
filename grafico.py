import matplotlib.pyplot as plt
import re
from collections import defaultdict

# Tempos sequenciais
tempo_sequencial = {
    "100M": 164.76,
    "1B": 4494.83
}

# Estrutura: tempos[algoritmo][N][np] = tempo
tempos = defaultdict(lambda: defaultdict(dict))

# Leitura do relatório
with open("relatorio_experimentos.txt") as f:
    lines = f.readlines()

algoritmo = ""
N = ""
np = None

for line in lines:
    stripped = line.strip()
    
    if stripped.startswith("mpi_"):
        algoritmo = stripped.rstrip(":")
    elif stripped in ("100M:", "1B:"):
        N = stripped.rstrip(":")
    elif re.match(r"^[0-9]+:$", stripped):
        np = int(stripped.rstrip(":"))
    elif "tempo" in stripped:
        tempo = float(re.search(r"([0-9.]+)", stripped).group(1))
        tempos[algoritmo][N][np] = tempo

# Lista de processadores
nps = sorted(set().union(*[tempos[alg][N].keys() for alg in tempos for N in ["100M", "1B"]]))

# Gráfico comparativo
plt.figure(figsize=(14, 7))
largura = 0.2
x = range(len(nps))

# Cálculo dos speed-ups
def get_speedup(alg, N):
    return [tempo_sequencial[N] / tempos[alg][N].get(np, float("inf")) for np in nps]

bar_offset = {
    "mpi_saco_100M": -1.5 * largura,
    "mpi_saco_1B": -0.5 * largura,
    "mpi_saltos_100M": 0.5 * largura,
    "mpi_saltos_1B": 1.5 * largura,
}

# Plotagem
for alg in ["mpi_saco", "mpi_saltos"]:
    for N in ["100M", "1B"]:
        label = f"{alg} - {N}"
        offset = bar_offset[f"{alg}_{N}"]
        plt.bar([i + offset for i in x], get_speedup(alg, N), width=largura, label=label)

# Customização
plt.xlabel("Número de Processadores")
plt.ylabel("Speed-Up")
plt.title("Comparação de Speed-Up: mpi_saco vs mpi_saltos (100M e 1B)")
plt.xticks(ticks=x, labels=nps)
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend()
plt.tight_layout()
plt.savefig("speedup_comparativo.png")
plt.show()
