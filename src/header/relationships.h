#include <stdio.h>

#define TAM_HASH 200
#define THDado "hash_dados.bin"
#define THash "hash_chaves.bin"

typedef enum{
    ACTED_IN,
    DIRECTED,
    PRODUCED,
    WROTE
}TipoRelacao;

typedef struct hash{
    int val;
    char filme[100];
    int end;
} TH;

typedef struct relacionamento{
    int val;
    char nome[50];
    TipoRelacao tipo; //0->ACTED_IN, 1->DIRECTED, 2->PRODUCED, 3->WROTE
    char papel[50];
    int prox;
} TR;

int hashit(char* nome);
void TH_inicializa();
void txt_to_hashMS(char *txt, char *arq);
//int TH_busca_filme(char *filme); // retorna o endereço filme x em hash_chaves.bin
int TH_busca_pessoa(char pessoa); // retorna o endereço da pessoa x em hash_dados.bin
void TH_insere(char *filme, TR *dado); 
int TR_busca_filme(char* filme); //retorna o endereco(offset) da primeira relacao do filme em hash_dados.bin
TR* TR_busca_elem(char* filme, char* nome_pessoa); // retorna o relacionamento filme->pessoa
int TR_busca_endereco_elem(char* filme, char* nome);