#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/TARVBMS.h"
#include "header/TD.h"

// Função auxiliar para poupar linhas na hora de inserir
void insere_pessoa(char *nome, int ano) {
    TD ent;
    ent.tipo = TIPO_PESSOA;
    strcpy(ent.dados.pessoa.nome, nome);
    ent.dados.pessoa.ano = ano;
    TARVBMS_insere(&ent);
}

int main() 
{
    int t = 2;
    TARVBMS_inicializa(t);
    TARVBMS_preenche_arvore("../entrada/Nodes.txt");
    TARVBMS_retira("The Matrix");
    TARVBMS_retira("Zach Grenier");
    TARVBMS_imprime();
    return 0;
}