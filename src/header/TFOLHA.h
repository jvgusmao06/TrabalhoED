#include "TD.h"

typedef struct{
    int nchaves; 
    char prox[30];
} TDADOSFOLHA;

typedef struct{
    TDADOSFOLHA dados;
    TD *entidades;
} TFOLHA;


TFOLHA *TFOLHA_le_folha(int index, int t);
TFOLHA *TFOLHA_carrega(char *nome);

void TFOLHA_cria(TFOLHA *folha, int t); // Cria um arquivo folha 
void TFOLHA_insere(TD *entidade, int t);