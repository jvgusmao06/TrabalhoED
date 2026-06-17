#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/TARVBMS.h"
#include "header/TD.h"
#include "header/interface.h"

// Função auxiliar para poupar linhas na hora de inserir
int main() 
{
    int t = escolhe_t();

    TARVBMS_inicializa(t);

    int e;
    while(1){
        printf("================================================================================================================================================\n");
        e = le_inteiro("Digite:\n0: para preencher\n1: para inserir\n2: para remover\n3: para imprimir a àrvore\n4: para ir para as questões\n-1: para parar o programa:\n");

        switch (e)
        {
        case 0:
            TARVBMS_preenche_arvore("../entrada/Nodes.txt");
            break;
        case 1:
            insere_entidades();
            break;
        case 2:
            remove_entidades();
            break;
        case 3:
            TARVBMS_imprime();
            break;
        case 4:
            printf("================================================================================================================================================\n");
            questoes();
            break;
        case -1:
            return 0;
        
        default:
            break;;
        }
    }

    return 0;
}