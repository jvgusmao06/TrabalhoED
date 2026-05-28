#include "../header/TP.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int ler_pessoa(char *arq, TP *p) {
    FILE *f = fopen(arq, "r");
    if(!f){
        printf("Ocorreu um erro ao ler o arquivo %s. Funcao ler_pessoa na TP.c\n", arq);
        exit(1);
    }
    
    char linha[256];
    while(fgets(linha, sizeof(linha), f) != NULL) {
        if (strncmp(linha, "Person | ", 9) == 0) {
            char nomeTemp[100];
            char espacoTemp[50]; 
            
            int lidos = sscanf(linha, "Person | %[^|]| %[^\n\r]", nomeTemp, espacoTemp);
            
            if(lidos == 2){
                int tam = strlen(nomeTemp);
                
                // Tira o espaco depois do nome, se tive
                if(tam > 0 && nomeTemp[tam-1] == ' '){
                    nomeTemp[tam-1] = '\0'; 
                }
                
                // Trata o espaco antes do ano
                char *anoStr = espacoTemp; 
                if (*anoStr == ' ') { 
                    anoStr++;
                }
                
                strcpy(p->nome, nomeTemp);
                
                // Converte e atribui o ano
                int anoint = atoi(anoStr);
                if(anoint == 0) {
                    p->ano = -1; // Coloca -1 se o ano de nascimento for invalido
                } else {
                    p->ano = anoint;
                }
            }
            fclose(f);
            return 1; // Retorna 1 se tiver sucesso lendo a pessoa
        }
    }

    fclose(f); 
    return 0;
}