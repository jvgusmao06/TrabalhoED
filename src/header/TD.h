#ifndef TD_H
#define TD_H

#include <stdio.h>

typedef enum {
    TIPO_PESSOA,
    TIPO_FILME
} TipoEntidade;

typedef struct {
    char nome[100];
    int ano;
} TP; // Tipo Pessoa

typedef struct {
    char titulo[100];
    int ano;
    char sub[200];
} TF; // Tipo Filme

// Struct unificada
typedef struct {
    TipoEntidade tipo; // Identificador de qual dado está preenchido
    union {
        TP pessoa;
        TF filme;
    } dados;
} TD;


int ler_entidade(FILE *f, TD *e);

#endif