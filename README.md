# SISTEMAS DISTRIBUIDOS

## Executando os scripts

### Preparação do ambiente python

Crie um virtual environment Python com:

```sh
python -m venv env
source env/bin/activate
pip install -r requirements.txt
```

ou, caso tenha virtualenv:

```sh
virtualenv env
source env/bin/activate
pip install -r requirements.txt
```

### Execução dos experimentos

Basta fazer, dentro de sua env:

```sh
python run_experimentos.py
```

este script irá:
    - baixar CMake, caso ainda não tenha
    - buildar CMake
    - compilar os .c
    - executar cada .c com cada argumento
    - salvar os logs em um subdiretório resultados

### Comparativo dos resultados

Todos os comparativos requisitados poderão ser observados ao se executar:

```sh
python analisar_resultados.py
```
