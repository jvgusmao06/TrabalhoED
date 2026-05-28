#include <stdio.h>
#include <stdlib.h>
#include "TD.h"

typedef struct arvbms{
    int folha;
    int nchaves;
    long *end_dados;
    long *filhos;
    long prox; // Posição da próxima folha no arquivo
} TARVBMS;

void TARVBMS_cria();
void TARVBMS_inicializa();
void TARVBMS_busca(TD *dado);
void TARVBMS_insere(TD *dado, int t);
void TARVBMS_retira(TD *dado, int t);
void TARVBMS_imprime();
void TARVBMS_imprime_chaves();