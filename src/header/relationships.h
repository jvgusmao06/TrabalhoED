#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define TAM_HASH 400

#define arqDado "hash_dados.bin"
#define arqHashFilme "hash_filme_chaves.bin"
#define arqHashPessoa "hash_pessoa_chaves.bin"
#define arqPosLivre "hash_livres.bin"

typedef enum{
    ACTED_IN,
    DIRECTED,
    PRODUCED,
    WROTE
}TipoRelacao;

typedef struct hashp{
    char nome_pessoa[50];
    int offset;
} THP;

typedef struct hashf{
    char filme[100];
    int offset;
} THF;

typedef struct relacionamentof{
    int valido;
    char nome_filme[100];
    char nome_pessoa[50];
    TipoRelacao tipo; //0->ACTED_IN, 1->DIRECTED, 2->PRODUCED, 3->WROTE
    char papel[50];
    int THFprox;
    int THPprox;
} TR;


// Hash e utilitários FOI!!!
int hashit(char* nome);
void trim(char* str);
void push_livre(int offset);
int pop_livre();

// Inicialização e preenchimento FOI!!!!
void TH_inicializa();
void preenche_hashes(char* txt);

// Utilitários de hash FOI!!!!
void escreve_hash_filme_vazio(int offset);
void escreve_hash_pessoa_vazio(int offset);
void atualiza_root_filme(int offset_hashNode, int offset_NewRoot);
void atualiza_root_pessoa(int offset_hashNode, int offset_NewRoot);

// Busca FOI!!!!!
int TR_busca_filme(char* filme);
int TR_busca_pessoa(char* pessoa);
int TR_busca_endereco(char* filme, char* pessoa, TipoRelacao tipo, char* papel);
int TR_busca_filme_anterior(FILE* fdados, char* filme, char* pessoa, TipoRelacao tipo, char* papel);
int TR_busca_pessoa_anterior(FILE* fdados, char* pessoa, char* filme, TipoRelacao tipo, char* papel);
int TR_busca_endereco_por_pessoa(char* filme, char* pessoa, TipoRelacao tipo, char* papel);

// Inserção FOI!!!!!
void THF_insere(TR* elem);
void THP_insere(TR* elem);

// Retirada FOI!!!
void TR_retira_relacao(char* filme, char* pessoa, TipoRelacao tipo, char* papel);
void TR_retira_relacionamento(FILE* arq, char* filme, char* pessoa, TipoRelacao tipo, char* papel);
void TR_retira_filme(char* filme);
void TR_retira_pessoa(char* pessoa);

// Impressão FOI!!!
void TR_imprime_relacoes_filme(char* filme);
void TR_menu();
#endif