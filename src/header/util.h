#ifndef UTIL_H
#define UTIL_H

#include "TD.h"
#include "TARVBMS.h"

typedef struct
{
    int t;
    long raiz;
    int indice;
    int preenchido;
} TConfig;

int compara_entidades_por_nome(TD *e1, TD *e2);
int compara_nomes_com_entidades(char *nome, TD *entidade);

void le_no_interno(FILE *f, long posicao, TARVBMSDADOS *infos, char (*ids)[TAM_NOME], long *filhos, char (*arq_folha)[TAM_ARQ_FOLHA]);
void trim(char *palavra);
#endif