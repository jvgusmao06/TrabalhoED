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
void txt_to_hashMS(char *txt, char *arq);
//int TH_busca_filme(char *filme); // retorna o endereço filme x em hash_chaves.bin
int TH_busca_pessoa(char* pessoa); // retorna o endereço da pessoa x em hash_dados.bin
int TR_busca_filme(char* filme); //retorna o endereco(offset) da primeira relacao do filme em hash_dados.bin
TR* TR_busca_elem(char* filme, char* nome_pessoa); // retorna o relacionamento filme->pessoa
int TR_busca_endereco_elem(char* filme, char* pessoa); // retorna endereco do primeiro relacionamento filme->pessoa
int TR_busca_endereco_por_papel(char* filme, char* pessoa, char* papel); // retorna offset do relacionamento filme->pessoa->papel
void TR_retira_relacao_por_nome(char* filme, char* nome_pessoa); // atualiza valor de validade da relacao para 0
void TR_retira_relacao_por_papel(char* filme, char* nome_pessoa, char* papel);

//Falta logica para inserir no primeiro espaco vazio disponivel.
void TH_insere(char *filme, TR *dado); 

//FALTA ESSA
void TR_retira_tudo(char* filme);