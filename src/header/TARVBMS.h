#ifndef TARVBMS_H
#define TARVBMS_H

#include "TFOLHA.h"
#include "TD.h"

#define TAM_NOME 100
#define TAM_SUB 200
#define TAM_ARQ_FOLHA 30

#define PESSOA 1
#define FILME 2

#define FILHO 0
#define FOLHA 1

typedef struct
{
    int t;
    long raiz;
    int indice;
    int preenchido;
} TConfig;
typedef struct
{
    int filho_folha;
    int val;
    int nchaves;
} TARVBMSDADOS;

typedef struct no
{
    TARVBMSDADOS infos;
    char **ids; // vetor de char nome[100]
    union
    {
        long *filhos;
        char **arq_folha; // vetor de char nome_arq[30];
    } dados;
} TARVBMS;

void TARVBMS_inicializa(int t); // inicializa o arquivo de indices

void TARVBMS_preenche_arvore(char *txt); // Preenche a árvore apartir de um arquivo txt

void TARVBMS_insere(TD *entidade); // Insere a entidade
void TARVBMS_retira(char *chave);  // Retira a entidade com o nome chave
void le_no_interno(FILE *f, long posicao, TARVBMSDADOS *infos, char (*ids)[TAM_NOME], long *filhos, char (*arq_folha)[TAM_ARQ_FOLHA]);
int compara_entidades_por_nome(TD *e1, TD *e2);

// Impressão
void TARVBMS_imprime();
void TARVBMS_imprime_folhas();

#endif