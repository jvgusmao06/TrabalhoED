#include <stdlib.h>

typedef struct{
    int id;
    char titulo[100];
    int ano;
    char sub[255];
}TF;
int ler_pessoa(FILE *f, TP *p);
