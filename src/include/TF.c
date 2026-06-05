#include "../header/TF.h"

int ler_filme(FILE *f, TF *p) {
    if(!f){
        printf("Erro no arquivo da funcao ler_filme!\n");
        exit(1);
    }
    
    char linha[500];
    while(fgets(linha, sizeof(linha), f) != NULL) {
        if (strncmp(linha, "Movie | ", 8) == 0) {
            char nomeTemp[100];
            char espacoTemp[50];
            char subTemp[200]; 
            
            int lidos = sscanf(linha, "Movie | %[^|]| %[^|]| %[^\n\r]", nomeTemp, espacoTemp, subTemp);
            
            if(lidos >= 2){
                int tam = strlen(nomeTemp);
                
                // Tira o espaco depois do titulo, se tive
                if(tam > 0 && nomeTemp[tam-1] == ' '){
                    nomeTemp[tam-1] = '\0'; 
                }
                
                // tira o espaco antes do ano
                char *anoStr = espacoTemp; 
                if (*anoStr == ' ') { 
                    anoStr++;
                }
                
                strcpy(p->titulo, nomeTemp);
                
                // Converte e atribui o ano
                int anoint = atoi(anoStr);
                if(anoint == 0) {
                    p->ano = -1; 
                } else {
                    p->ano = anoint;
                }
                
                // Se leu a tagline (lidos == 3), remove o espaço inicial e copia
                if(lidos == 3){
                    char *subStr = subTemp;
                    if (*subStr == ' ') {
                        subStr++;
                    }
                    strcpy(p->sub, subStr);
                } else {
                    strcpy(p->sub, "");
                }
            }
            
            return 1; // Retorna 1 se tiver sucesso lendo o filme
        }
    }

    return 0; // Fim do arquivo
}
