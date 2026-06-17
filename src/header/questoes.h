#ifndef QUESTOES_H
#define QUESTOES_H

#include "TARVBMS.h"
#include "relationships.h"

typedef struct {
    char pessoaA[50];
    char pessoaB[50];
    int count;
} Par;

typedef struct {
    char pessoa[50];
    int count;
} Frequencia;

void insere_par(char* a, char* b, int* npares, int* cap_par, Par** pares);
void QA();
void QB();
void QC();

#endif 