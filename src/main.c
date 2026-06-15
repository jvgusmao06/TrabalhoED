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
    insere_pessoa("Aini goat",10);
    insere_pessoa("Jini goat",10);
    insere_pessoa("Kini goat",10);
    insere_pessoa("Lini goat",10);
    TARVBMS_retira("Laurence Fishburne");
    TARVBMS_retira("Jini goat");
    TARVBMS_retira("Lana Wachowski");
    TARVBMS_retira("Kini goat");
    TARVBMS_retira("Keanu Reeves");
    TARVBMS_retira("The Devil's Advocate");
    TARVBMS_retira("The Matrix Reloaded");
    TARVBMS_retira("The Matrix Revolutions");
    TARVBMS_retira("Tom Cruise");





    printf("\n\n\n\n\n");
    TARVBMS_imprime();
    //TARVBMS_imprime_folhas();
    return 0;
}