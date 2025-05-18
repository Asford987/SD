import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

sns.set(style="whitegrid")
os.makedirs("resultados/analises", exist_ok=True)

# ---------- Carregar dados ----------
df = pd.read_csv("resultados/tempos_completos.csv")
df["eficiencia"] = df["speedup"] / df["np"]

# ---------- Gráficos ----------
def plot_linha(df, y, ylabel, titulo, nome_arquivo):
    plt.figure(figsize=(10, 6))
    for versao in df["versao"].unique():
        for N in sorted(df["N"].unique()):
            subset = df[(df["versao"] == versao) & (df["N"] == N)]
            label = f"{versao.replace('mpi_', '')} - N={N:,}"
            plt.plot(subset["np"], subset[y], label=label)
    plt.xlabel("Número de Processos")
    plt.ylabel(ylabel)
    plt.title(titulo)
    plt.legend()
    plt.xticks(df["np"].unique())
    plt.savefig(f"resultados/analises/{nome_arquivo}")
    plt.close()
    print(f"Gráfico salvo: {nome_arquivo}")

plot_linha(df, "tempo", "Tempo (s)", "Tempo de Execução por Versão", "tempo_execucao.png")
plot_linha(df, "speedup", "Speedup", "Speedup por Versão", "speedup_linhas.png")
plot_linha(df, "eficiencia", "Eficiência", "Eficiência por Versão", "eficiencia.png")

# ---------- Estatísticas ----------
summary = df.groupby(["versao", "N"]).agg({
    "tempo": ["min", "max", "mean"],
    "speedup": ["min", "max", "mean"],
    "eficiencia": ["min", "max", "mean"]
}).round(2)

# ---------- Exportar tabela ----------
summary.to_csv("resultados/analises/resumo_estatistico.csv")
print("Tabela resumo salva: resumo_estatistico.csv")

# ---------- Mostrar parte da tabela no terminal ----------
print("\nResumo Estatístico:")
print(summary)
