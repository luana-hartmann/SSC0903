import os
import re
import numpy as np
import pandas as pd

# Diretório onde estão os arquivos .out
RESULT_DIR = "outputs"

# Regex para extrair os números (pode ajustar conforme o output do seu programa)
NUM_REGEX = re.compile(r"[-+]?\d*\.\d+|\d+")

# Dicionário para armazenar resultados: { (programa, caso): [valores] }
resultados = {}

# Percorre todos os arquivos .out
for filename in os.listdir(RESULT_DIR):
    if filename.endswith(".out"):
        caminho = os.path.join(RESULT_DIR, filename)

        # Extrai nome do programa e caso de teste
        base = filename.replace(".out", "")
        partes = base.split("_", 1)
        if len(partes) != 2:
            continue  # ignora arquivos mal formatados
        programa, caso = partes

        with open(caminho, "r") as f:
            conteudo = f.read()

        # Extrai todos os números do arquivo
        numeros = list(map(float, NUM_REGEX.findall(conteudo)))
        if not numeros:
            continue

        media = np.mean(numeros)
        desvio = np.std(numeros)

        resultados[(caso, programa)] = (media, desvio)

# Organiza os dados em um DataFrame
casos = sorted(set(c for c, _ in resultados.keys()))
programas = sorted(set(p for _, p in resultados.keys()))

tabela = pd.DataFrame(index=casos, columns=programas)

for (caso, programa), (media, desvio) in resultados.items():
    tabela.loc[caso, programa] = f"{media:.4f} ± {desvio:.4f}"

# Imprime a tabela
print("\n📊 Tabela de Resultados (média ± desvio padrão):\n")
print(tabela.to_markdown(tablefmt="github"))

# Salva como CSV (opcional)
# tabela.to_csv("resumo_resultados.csv")
