#include <stdlib.h>
#include <stdio.h>

typedef struct filme{
    int id;
    char titulo[100];
    int ano;
    char sub[255];
}TF;

int ler_filme(FILE *f, TF *p);
