#include <stdio.h>
#include <stdlib.h>
#include "TD.h"
#include "TF.h"

#define PESSOA 1
#define FILME 2

typedef struct{
    int t;
    long raiz;
} TConfig;

typedef struct folha{
    int val; // valido(1) ou invalido(0)
    int *tipo; // PESSOA(1) ou FILME(2)
    int nchaves; 
    TD **entidades;
    char **prox;
} TFOLHA;

typedef struct no{
    int val;
    int filho_folha;
    int nchaves;
    char **ids;
    long *filhos;
    char **arq_folha;
} NoIndice;

void TARVBMS_cria();
void TARVBMS_inicializa();
void TARVBMS_busca(TD *dado);
void TARVBMS_insere(TD *dado, int t);
void TARVBMS_retira(TD *dado, int t);
void TARVBMS_imprime();
void TARVBMS_imprime_chaves();
long TARVBMS_raiz(FILE* arq_index);