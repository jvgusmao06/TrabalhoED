#include "../header/TD.h"
#include <stdlib.h>
#include <string.h>

TD *TD_aloca(){
    TD *novo = (TD*) malloc(sizeof(TD));
    return novo;
}

int ler_entidade(FILE *f, TD *e) {
    if (!f) {
        printf("Erro no arquivo da funcao ler_entidade!\n");
        exit(1);
    }
    
    char linha[500];
    
    while (fgets(linha, sizeof(linha), f) != NULL) {
        
        // Verifica se a linha é do tipo Pessoa
        if (strncmp(linha, "Person | ", 9) == 0) {
            e->tipo = TIPO_PESSOA;
            char nomeTemp[100];
            char espacoTemp[50]; 
            
            int lidos = sscanf(linha, "Person | %[^|]| %[^\n\r]", nomeTemp, espacoTemp);
            
            if (lidos == 2) {
                int tam = strlen(nomeTemp);
                
                // Tira o espaco depois do nome, se tiver
                if (tam > 0 && nomeTemp[tam-1] == ' ') {
                    nomeTemp[tam-1] = '\0'; 
                }
                
                // Trata o espaco antes do ano
                char *anoStr = espacoTemp; 
                if (*anoStr == ' ') { 
                    anoStr++;
                }
                
                strcpy(e->dados.pessoa.nome, nomeTemp);
                
                // Converte e atribui o ano
                int anoint = atoi(anoStr);
                if (anoint == 0) {
                    e->dados.pessoa.ano = -1; // Coloca -1 se o ano for invalido
                } else {
                    e->dados.pessoa.ano = anoint;
                }
            }
            return 1; // Retorna 1 se teve sucesso lendo a pessoa
        }
        
        // Verifica se a linha é do tipo Filme
        else if (strncmp(linha, "Movie | ", 8) == 0) {
            e->tipo = TIPO_FILME;
            char nomeTemp[100];
            char espacoTemp[50];
            char subTemp[200]; 
            
            int lidos = sscanf(linha, "Movie | %[^|]| %[^|]| %[^\n\r]", nomeTemp, espacoTemp, subTemp);
            
            if (lidos >= 2) {
                int tam = strlen(nomeTemp);
                
                // Tira o espaco depois do titulo, se tiver
                if (tam > 0 && nomeTemp[tam-1] == ' ') {
                    nomeTemp[tam-1] = '\0'; 
                }
                
                // Tira o espaco antes do ano
                char *anoStr = espacoTemp; 
                if (*anoStr == ' ') { 
                    anoStr++;
                }
                
                strcpy(e->dados.filme.titulo, nomeTemp);
                
                // Converte e atribui o ano
                int anoint = atoi(anoStr);
                if (anoint == 0) {
                    e->dados.filme.ano = -1; 
                } else {
                    e->dados.filme.ano = anoint;
                }
                
                // Se leu a tagline (lidos == 3), remove o espaco inicial e copia
                if (lidos == 3) {
                    char *subStr = subTemp;
                    if (*subStr == ' ') {
                        subStr++;
                    }
                    strcpy(e->dados.filme.sub, subStr);
                } else {
                    strcpy(e->dados.filme.sub, "");
                }
            }
            return 1; // Retorna 1 se teve sucesso lendo o filme
        }
    }

    return 0; // Fim do arquivo
}
