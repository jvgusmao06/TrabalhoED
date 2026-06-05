#include "../header/TARVBMS.h"





void TARVBMS_inicializa(int t){ // Cria todos os arquivos vazios
    FILE *f = fopen("../../dados/index.bin", "wb");

    if(!f){
        printf("Falha ao criar arquivo index. Funcao TARVBMS inicializa\n");
        exit(1);
    }
    TConfig temp;
    temp.raiz = -1;
    temp.t = t;
    fwrite(&temp, sizeof(TConfig), 1, f);
    FILE *fo = fopen("../../dados/folha.bin", "wb");
    if(!fo){
        printf("Falha ao criar arquivo folhas!! Funcao TARVBMS inicializa\n");
        exit(1);
    }
    //fwrite(&raiz, sizeof(int), 1, fo);
    fclose(fo);
    fclose(f);
}


long TARVBMS_raiz(FILE* arq){
    long raiz;
    TConfig temp;
    fseek(arq, 0, SEEK_SET);
    fread(&temp, sizeof(TConfig), 1, arq);
    if(temp.raiz <= 0){
        printf("Raiz nao encontrada!! Funcao TARVBMS_raiz\n");
        return -1;
    }
    raiz = temp.raiz;
    return raiz;
}