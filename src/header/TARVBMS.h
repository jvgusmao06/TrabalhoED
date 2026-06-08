#include <stdio.h>
#include "TD.h"

#define PESSOA 1
#define FILME 2

#define FILHO 0
#define FOLHA 1

typedef struct{
    int t;
    long raiz;
    int indice;
} TConfig;

typedef struct{
    TDADOSFOLHA dados;
    TD *entidades;
} TFOLHA;

typedef struct{
    int nchaves; 
    char prox[30];
} TDADOSFOLHA;

typedef struct no{
    TARVBMSDADOS infos;
    char **ids; // vetor de char nome[100]
    union{
        long *filhos;
        char **arq_folha; // vetor de char nome_arq[30];
    } dados;
} TARVBMS;

typedef struct{
    int filho_folha;
    int val;
    int nchaves;
} TARVBMSDADOS;

void TARVBMS_inicializa(); // inicializa o arquivo de indices
TFOLHA TARVBMS_busca(char *nome, int t);

void TARVBMS_insere_um(TD *entidade, int t);
void TARVBMS_retira(TD *dado, int t);
void TARVBMS_insere_um_do_arquivotxt(char *nome);
long TARVBMS_raiz(FILE* arq_index);

void TARVBMS_imprime();
void TARVBMS_imprime_chaves();