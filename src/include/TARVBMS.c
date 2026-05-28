#include "../header/TARVBMS.h"

void TARBMS_cria(int t){
    
}

void TARVBMS_inicializa(){ // Cria todos os arquivos vazios
    FILE *f = fopen("../../dados/index.bin", "wb");

    if(!f){
        exit(1);
    }
    fclose(f);
}
