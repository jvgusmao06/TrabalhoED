#include <stdio.h>
#include <stdlib.h>

typedef struct arvbms{
    int folha;
    int nchaves;
    // O dado vai ser o nome ou um indice?
    long prox; // Posição da próxima folha no arquivo
    // Vetor de filhos
} TARVBMS;