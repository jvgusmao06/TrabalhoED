#ifndef TFOLHA_H
#define TFOLHA_H

#include "TD.h"

typedef struct
{
    int nchaves;
    char prox[30];
} TDADOSFOLHA;

typedef struct
{
    TDADOSFOLHA dados;
    TD *entidades;
} TFOLHA;

// Funções para memoria principal
TFOLHA *TFOLHA_aloca(int t);         // Aloca uma folha na memoria
TFOLHA *TFOLHA_le(int index, int t); // Le o arquivo folha do index e aloca ela na memoria
TFOLHA *TFOLHA_carrega(char *nome); // Carre a folha com o nome na memoria

void TFOLHA_imprime(TFOLHA *f); // Imprime as chaves da folha

// Funções para memoria secundaria
void TFOLHA_cria(TFOLHA *folha, int t, int index);  // Cria um arquivo folha e preenche ele com o TFOLHA
void TFOLHA_insere(TD *entidade, int t, int index); // Insere um TD no arquivo folha de indice index

#endif