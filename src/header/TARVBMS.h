#ifndef TARVBMS_H
#define TARVBMS_H

#include "TFOLHA.h"
#include "TD.h"

#define TAM_NOME 100
#define TAM_ARQ_FOLHA 30

#define PESSOA 1
#define FILME 2

#define FILHO 0
#define FOLHA 1

typedef struct{
    int filho_folha;
    int val;
    int nchaves;
} TARVBMSDADOS;

typedef struct no{
    TARVBMSDADOS infos;
    char **ids; // vetor de char nome[100]
    union{
        long *filhos;
        char **arq_folha; // vetor de char nome_arq[30];
    } dados;
} TARVBMS;

long TARVBMS_raiz(FILE* arq_index);

void TARVBMS_inicializa(int t); // inicializa o arquivo de indices

void TARVBMS_insere_um(TD *entidade);
void TARVBMS_preenche_arvore(char* txt);
//void TARVBMS_insere_um_do_arquivotxt(char *nome, int t);

void TARVBMS_divisao_folha(FILE *f, long pos_no, int i_filho, int indice_folha);
void TARVBMS_retira(char *chave);
int TARVBMS_remove_da_folha(int indice_folha, char *chave, int t);
void TARVBMS_removezada(FILE *f, long pos_no, char*chave, int t);

// Impressão
void TARVBMS_imprime();
void TARVBMS_imprime_folhas();

#endif