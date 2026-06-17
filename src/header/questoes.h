#include "src/header/TARVBMS.h"
#include "src/header/relationships.h"

typedef struct {
    char pessoaA[50];
    char pessoaB[50];
    int count;
} Par;

typedef struct {
    char pessoa[50];
    int count;
} Frequencia;

void todos_trab_juntos();
void insere_par(char* a, char* b, int* npares, int* cap_par, Par** pares);
