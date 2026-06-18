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
void QF();
void QG();
void QH();
void QI();
void QJ();
void QK();
void QO();
void QP();
void QM();
void QT();

#endif 