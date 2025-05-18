import subprocess
import os
import sys
import shutil
import time
import csv

# Instala dependências de sistema (Linux apt-based)
def instalar_dependencias():
    print("Verificando dependências do sistema...")
    try:
        subprocess.run(["cmake", "--version"], stdout=subprocess.DEVNULL)
    except FileNotFoundError:
        print("Instalando CMake...")
        subprocess.run(["sudo", "apt", "install", "-y", "cmake"])

    try:
        subprocess.run(["mpicc", "--version"], stdout=subprocess.DEVNULL)
    except FileNotFoundError:
        print("Instalando OpenMPI...")
        subprocess.run(["sudo", "apt", "install", "-y", "openmpi-bin", "libopenmpi-dev"])

# Instala bibliotecas Python
def instalar_dependencias_python():
    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("Instalando matplotlib...")
        subprocess.run([sys.executable, "-m", "pip", "install", "matplotlib"])

# Build com CMake
def build_projeto():
    print("Compilando projeto com CMake...")
    os.makedirs("build", exist_ok=True)
    subprocess.run(["cmake", ".."], cwd="build")
    subprocess.run(["make"], cwd="build")

# Executa uma versão e retorna o tempo
def executar_comando(cmd):
    start = time.time()
    subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    end = time.time()
    return end - start

# Executa a versão sequencial e retorna tempos
def executar_sequencial(bin_path, Ns):
    tempos = {}
    for n in Ns:
        print(f"SEQ | N={n}")
        tempo = executar_comando([bin_path, str(n)])
        tempos[n] = tempo
        print(f"     {tempo:.2f}s")
    return tempos

# Executa uma versão MPI com diferentes processos
def executar_mpi(bin_path, versao, Ns, processos, tempos_seq):
    dados = []
    for n in Ns:
        for np in processos:
            print(f" MPI | {versao} | N={n} | np={np}")
            tempo = executar_comando(["mpirun", "-np", str(np), bin_path, str(n)])
            speedup = tempos_seq[n] / tempo
            dados.append({"versao": versao, "N": n, "np": np, "tempo": tempo, "speedup": speedup})
            print(f"     {tempo:.2f}s | Speedup: {speedup:.2f}")
    return dados

# Salva CSV
def salvar_csv(nome, dados):
    with open(nome, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=["versao", "N", "np", "tempo", "speedup"])
        writer.writeheader()
        writer.writerows(dados)

# Gera gráfico final
def gerar_grafico(dados, saida_img):
    import matplotlib.pyplot as plt

    plt.figure(figsize=(10, 6))
    cores = {"mpi_saltos": "blue", "mpi_saco": "green"}
    linhas = {"mpi_saltos": "-", "mpi_saco": "--"}

    for versao in ["mpi_saltos", "mpi_saco"]:
        for n in sorted(set(d["N"] for d in dados)):
            subset = [d for d in dados if d["versao"] == versao and d["N"] == n]
            x = [d["np"] for d in subset]
            y = [d["speedup"] for d in subset]
            label = f"{versao.replace('mpi_', '')} - N={n:,}"
            plt.plot(x, y, label=label, linestyle=linhas[versao], color=cores[versao])

    plt.xlabel("Número de Processos")
    plt.ylabel("Speedup")
    plt.title("Speedup das Versões MPI")
    plt.grid(True)
    plt.legend()
    plt.savefig(saida_img)
    plt.show()
    print(f"Gráfico salvo em {saida_img}")

# ================== EXECUÇÃO ==================
if __name__ == "__main__":
    instalar_dependencias()
    instalar_dependencias_python()

    bin_dir = "build/bin"
    os.makedirs("resultados", exist_ok=True)

    # Compilar
    if not os.path.exists(f"{bin_dir}/seq_primos"):
        build_projeto()

    Ns = [100_000_000, 1_000_000_000]
    processos = list(range(2, 17))

    tempos_seq = executar_sequencial(f"{bin_dir}/seq_primos", Ns)

    resultados = []
    for versao in ["mpi_saltos", "mpi_saco"]:
        bin_path = f"{bin_dir}/{versao}"
        dados = executar_mpi(bin_path, versao, Ns, processos, tempos_seq)
        salvar_csv(f"resultados/{versao}.csv", dados)
        resultados.extend(dados)

    salvar_csv("resultados/tempos_completos.csv", resultados)
    gerar_grafico(resultados, "resultados/speedup.png")
