#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define TAM_HASH 200
#define arqDado "hash_dados.bin"
#define arqHash "hash_chaves.bin"
#define arqPosLivre "hash_livres.bin"

typedef enum{
    ACTED_IN,
    DIRECTED,
    PRODUCED,
    WROTE
}TipoRelacao;

// Duvida pro grupo: Vamos aderir tabela hash com esse no pra cada elemento ou
// usamos abordagem de tamanho fixo e indexacao por inteiros (hash da string)?
typedef struct hash{
    int valido;
    char filme[100];
    int end;
} TH;

typedef struct relacionamento{
    int valido;
    char nome[50];
    TipoRelacao tipo; //0->ACTED_IN, 1->DIRECTED, 2->PRODUCED, 3->WROTE
    char papel[50];
    int prox;
} TR;

int hashit(char* nome);
void TH_inicializa();
void trim(char* str);
void txt_to_hashMS(char *txt);
//int TH_busca_filme(char *filme); // retorna o endereço filme x em hash_chaves.bin
int TH_busca_pessoa(char* pessoa); // retorna o endereço da pessoa x em hash_dados.bin
int TR_busca_filme(char* filme); //retorna o endereco(offset) da primeira relacao do filme em hash_dados.bin
TR* TR_busca_elem(char* filme, char* nome_pessoa); // retorna o relacionamento filme->pessoa
int TR_busca_endereco_elem(char* filme, char* pessoa); // retorna endereco do primeiro relacionamento filme->pessoa
int TR_busca_endereco_por_papel(char* filme, char* pessoa, char* papel); // retorna offset do relacionamento filme->pessoa->papel
int TR_busca_anterior(FILE* fdados, char* filme, char* nome_pessoa);
int TR_busca_anterior_por_papel(FILE* fdados, char* filme, char* nome_pessoa, char* papel);
void TR_retira_relacao_por_nome(char* filme, char* nome_pessoa); // atualiza valor de validade da relacao para 0
void TR_retira_relacao_por_papel(char* filme, char* nome_pessoa, char* papel);
void TR_insere(char *filme, TR *dado); 
void TR_retira_tudo(char* filme);
void push_livre(int offset); // bota endereco do espaco vazio no arquivo de espacos livres
int pop_livre(); // pega endereco no topo da pilha (arquivo) de enderecos vazios
void TR_imprime_relacoes_filme_pessoa(char* filme, char* pessoa);
void TR_imprime_relacoes_filme(char* filme);
void TR_menu();