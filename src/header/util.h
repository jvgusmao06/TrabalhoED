#ifndef UTIL_H
#define UTIL_H

#include "TD.h"
#include "TARVBMS.h"

typedef struct{
    int t;
    long raiz;
    int indice;
} TConfig;

int compara_entidades_por_nome(TD *e1, TD *e2);
int compara_nomes_com_entidades(char *nome, TD *entidade);

void le_no_interno(FILE *f, long posicao, TARVBMSDADOS *infos, char (*ids)[100], long *filhos, char (*arq_folha)[30]);
void trim(char* palavra);
#endif