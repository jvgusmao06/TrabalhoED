#include <stdio.h>
#include <stdlib.h>

typedef struct pessoa{
    int id;
    char nome[100];
    int ano;
}TP;

int ler_pessoa(char *arq, TP *p);