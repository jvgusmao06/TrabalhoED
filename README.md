# Banco de Dados de Filmes e Pessoas em Memória Secundária (Árvore B+ e Tabela Hash)

Este projeto implementa um sistema de gerenciamento de banco de dados em **memória secundária (disco)** na linguagem C, desenvolvido como trabalho prático da disciplina de **Estruturas de Dados (UFF)**.

O sistema modela um grafo de **Filmes** e **Pessoas** (atores, diretores, produtores e roteiristas), armazenando as entidades em uma **Árvore B+** e os relacionamentos entre elas em **Tabelas Hash com encadeamento externo em arquivo**.

---

## Sumário

- [Arquitetura e Estruturas de Dados](#arquitetura-e-estruturas-de-dados)
  - [1. Entidades (`TD`)](#1-entidades-td)
  - [2. Árvore B+ em Memória Secundária (`TARVBMS` e `TFOLHA`)](#2-árvore-b-em-memória-secundária-tarvbms-e-tfolha)
  - [3. Tabelas Hash de Relacionamentos (`TR`, `THF`, `THP`)](#3-tabelas-hash-de-relacionamentos-tr-thf-thp)
- [Estrutura de Diretórios](#estrutura-de-diretórios)
- [Formato dos Arquivos de Entrada](#formato-dos-arquivos-de-entrada)
- [Funcionalidades e Menus](#funcionalidades-e-menus)
  - [Menu Principal](#menu-principal)
  - [Menu de Relacionamentos (Hash)](#menu-de-relacionamentos-hash)
  - [Consultas e Questões Implementadas](#consultas-e-questões-implementadas)
- [Como Compilar e Executar](#como-compilar-e-executar)
- [Autores](#autores)

---

## Arquitetura e Estruturas de Dados

### 1. Entidades (`TD`)
As entidades do banco são representadas por uma estrutura unificada (`TD`) que utiliza uma `union` discriminada pelo enumerador `TipoEntidade`:
- **Pessoa (`TP`)**: Nome (`char[100]`) e Ano de nascimento (`int`).
- **Filme (`TF`)**: Título (`char[100]`), Ano de lançamento (`int`) e Subtítulo/Tagline (`char[200]`).

A chave de indexação e ordenação na Árvore B+ é o **nome da pessoa** ou o **título do filme** (comparados de forma *case-insensitive*).

### 2. Árvore B+ em Memória Secundária (`TARVBMS` e `TFOLHA`)
A Árvore B+ possui grau mínimo `t` configurável pelo usuário na inicialização do programa (mínimo `t = 2`), comportando até `2t - 1` chaves e `2t` filhos por nó:
- **Arquivo de Índice (`dados/index.bin`)**: Armazena o cabeçalho de configuração da árvore (`TConfig`: grau `t`, offset da raiz, contador de folhas) seguido pelos nós internos da Árvore B+.
- **Arquivos de Folha (`dados/folha_<id>.bin`)**: Cada folha da Árvore B+ é persistida em seu próprio arquivo binário contendo os registros completos das entidades (`TD`) ordenados alfabeticamente e o nome do arquivo da próxima folha (`prox`), formando uma lista encadeada de folhas para varreduras sequenciais rápidas.

### 3. Tabelas Hash de Relacionamentos (`TR`, `THF`, `THP`)
Os relacionamentos entre pessoas e filmes (`ACTED_IN`, `DIRECTED`, `PRODUCED`, `WROTE`) são gerenciados em disco no diretório `saida/` por meio de uma estrutura de **multi-lista encadeada indexada por duas Tabelas Hash** de tamanho `TAM_HASH = 400`:
- **`saida/hash_filme_chaves.bin` (`THF`)**: Tabela hash indexada pelo título do filme, apontando para o primeiro relacionamento daquele filme em `hash_dados.bin`.
- **`saida/hash_pessoa_chaves.bin` (`THP`)**: Tabela hash indexada pelo nome da pessoa, apontando para o primeiro relacionamento daquela pessoa em `hash_dados.bin`.
- **`saida/hash_dados.bin` (`TR`)**: Arquivo de dados contendo os registros de relacionamento (filme, pessoa, tipo da relação, papel interpretado e ponteiros de próximo offset `THFprox` e `THPprox`).
- **`saida/hash_livres.bin`**: Pilha em disco que gerencia os *offsets* de registros removidos logicamente em `hash_dados.bin`, permitindo o reaproveitamento de espaço nas novas inserções.

---

## Estrutura de Diretórios

```text
TrabalhoED/
├── dados/                        # Arquivos binários gerados pela Árvore B+
│   ├── index.bin                 # Cabeçalho e nós internos da Árvore B+
│   └── folha_*.bin               # Nós folhas contendo as entidades (Pessoas e Filmes)
├── entrada/                      # Base de dados de entrada em formato texto
│   ├── Nodes.txt                 # Lista de filmes (Movie) e pessoas (Person)
│   └── Relationships.txt         # Lista de relacionamentos entre pessoas e filmes
├── saida/                        # Arquivos binários gerados pelas Tabelas Hash
│   ├── hash_dados.bin            # Registros de relacionamentos
│   ├── hash_filme_chaves.bin     # Índice hash por Filme
│   ├── hash_pessoa_chaves.bin    # Índice hash por Pessoa
│   └── hash_livres.bin           # Controle de posições livres para reuso
├── src/                          # Código-fonte do projeto
│   ├── main.c                    # Ponto de entrada do programa
│   ├── header/                   # Arquivos de cabeçalho (.h)
│   │   ├── TARVBMS.h             # Interface da Árvore B+ em memória secundária
│   │   ├── TFOLHA.h              # Interface de manipulação das folhas
│   │   ├── TD.h                  # Definição dos tipos de dados (Pessoa / Filme)
│   │   ├── relationships.h       # Interface das Tabelas Hash de relacionamentos
│   │   ├── questoes.h            # Declaração das consultas analíticas
│   │   └── interface.h           # Funções de menu e interação com o usuário
│   └── include/                  # Implementações (.c)
│       ├── TARVBMS.c
│       ├── TFOLHA.c
│       ├── TD.c
│       ├── relationships.c
│       ├── questoes.c
│       └── interface.c
└── README.md
```

---

## Formato dos Arquivos de Entrada

### `entrada/Nodes.txt`
Define as entidades a serem carregadas na Árvore B+:
```text
Movie | <Título> | <Ano de Lançamento> | <Subtítulo / Tagline>
Person | <Nome> | <Ano de Nascimento>
```
**Exemplo:**
```text
Movie | The Matrix | 1999 | Welcome to the Real World
Person | Keanu Reeves | 1964
```

### `entrada/Relationships.txt`
Define as arestas (relacionamentos) a serem carregadas nas Tabelas Hash:
```text
START Person | <Nome> | <TIPO_RELACAO> | END Movie | <Título> [| role: <Papel>]
```
**Exemplo:**
```text
START Person | Keanu Reeves | ACTED_IN | END Movie | The Matrix | role: Neo
START Person | Lilly Wachowski | DIRECTED | END Movie | The Matrix
```

---

## Funcionalidades e Menus

Ao iniciar o programa, o usuário escolhe o grau mínimo `t` da Árvore B+ e acessa o menu principal.

### Menu Principal
- **`0` - Preencher**: Carrega automaticamente todas as entidades de `../entrada/Nodes.txt` na Árvore B+ e todos os relacionamentos de `../entrada/Relationships.txt` nas Tabelas Hash.
- **`1` - Inserir**: Permite inserir manualmente novas Pessoas ou Filmes na Árvore B+.
- **`2` - Remover**: Remove uma entidade (Pessoa ou Filme) da Árvore B+ a partir de seu nome/título.
- **`3` - Buscar uma Folha**: Busca uma folha pelo nome/título digitado pelo usuário, imprimindo a folha encontrada e todos os dados da pessoa ou filme.
- **`4` - Imprimir a Árvore**: Exibe a estrutura atual da Árvore B+ e suas folhas.
- **`5` - Ir para as Questões**: Abre o seletor de consultas sobre a base de dados.
- **`6` - Menu da Hash**: Abre o submenu dedicado à manipulação direta de relacionamentos.
- **`-1` - Parar o programa**: Encerra a execução.

### Menu de Relacionamentos (Hash)
Acessado pela opção `6` do menu principal:
1. Inserir novo relacionamento entre pessoa e filme (`ACTED_IN`, `DIRECTED`, `PRODUCED`, `WROTE`).
2. Buscar e listar todos os relacionamentos de um **filme**.
3. Buscar e listar todos os relacionamentos de uma **pessoa**.
4. Buscar o(s) relacionamento(s) específico(s) entre uma **pessoa e um filme**.
5. Remover um relacionamento específico.
6. Remover todos os relacionamentos de um **filme**.
7. Remover todos os relacionamentos de uma **pessoa**.

### Consultas e Questões Implementadas
Acessadas pela opção `5` do menu principal digitando a letra correspondente:

| Letra | Descrição da Consulta |
| :---: | :--- |
| **`a`** | Lista todos os pares de pessoas que trabalharam juntas no mesmo filme (em qualquer função) e a quantidade de vezes. |
| **`b`** | Lista os pares de pessoas que trabalharam juntas como **ator/atriz ou diretor(a)** no mesmo filme e a frequência. |
| **`c`** | Lista os pares de **atores/atrizes** que atuaram juntos no mesmo filme e a quantidade de colaborações. |
| **`f`** | Exibe o Top 3 de **atores que mais atuaram**, acompanhado da lista de filmes de cada um. |
| **`g`** | Exibe o Top 3 de **atores que menos atuaram**, acompanhado da lista de filmes de cada um. |
| **`h`** | Exibe o Top 3 de **diretores que mais dirigiram**, acompanhado da lista de filmes de cada um. |
| **`i`** | Exibe o Top 3 de **diretores que menos dirigiram**, acompanhado da lista de filmes de cada um. |
| **`j`** | Exibe o Top 3 de **produtores que mais produziram**, acompanhado da lista de filmes de cada um. |
| **`k`** | Exibe o Top 3 de **produtores que menos produziram**, acompanhado da lista de filmes de cada um. |
| **`m`** | Identifica e lista os **filmes que possuem continuações** na base (comparando o prefixo do título e ignorando artigos iniciais como *The*, *A*, *O*, etc.). |
| **`o`** | Lista todas as pessoas que **atuaram e também dirigiram** filmes. |
| **`p`** | Lista todas as pessoas que **atuaram e também produziram** filmes. |
| **`t`** | Lista os **atores/pessoas nascidos no mesmo ano de lançamento de algum filme** cadastrado na Árvore B+. |

*(Nota: A função `QN`, que agrupa pessoas nascidas no mesmo ano, também encontra-se implementada em `src/include/questoes.c`).*

---

## Como Compilar e Executar

> **Importante:** O programa utiliza caminhos relativos (`../dados/`, `../entrada/`, `../saida/`), portanto deve ser compilado e executado a partir do diretório `src/`, garantindo que as pastas `dados/` e `saida/` existam na raiz do projeto.

### 1. Acesse o diretório `src`
```bash
cd src
```

### 2. Compile o projeto com `gcc`
```bash
gcc main.c include/TARVBMS.c include/TFOLHA.c include/TD.c include/interface.c include/relationships.c include/questoes.c -o main
```

### 3. Execute o binário
No Linux / macOS / Git Bash:
```bash
./main
```
No Windows (PowerShell / CMD):
```powershell
.\main.exe
```

---

## Autores

- **Carlos Seiji Yamasaki Magno**
- **João Vitor Gusmão de Souza**
- **Rafael Gusmão Corrêa**
