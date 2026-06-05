#include <stdio.h>

#define ACTED_IN 0
#define DIRECTED 1

typedef struct{
    int val;
    char filme[50];
    int end;
} TH;

typedef struct{
    int val;
    char nome[50];
    int *tipo;
    char ** papeis;
    int prox;
} TR;

void txt_to_hashMS(char *txt, char *arq);
int TH_busca_filme(char *filme); // retorna o endereço filme x em hash_chaves.bin
int TH_busca_pessoa(char pessoa); // retorna o endereço da pessoa x em hash_dados.bin
void TH_insere(char *filme, TR *dado); 
