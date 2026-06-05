#include "../header/relationships.h"
#include <string.h>

void txt_to_hashMS(char *txt, char *arq){
    FILE *t = fopen(txt, "r");
    FILE *f = fopen(arq, "wb");
    if(!t || !f) exit(1);

    char linha[256];
    while(fgets(linha, sizeof(linha), t)){
        //sprintf(linha, "START Person | Meg Ryan | ACTED_IN | END Movie | When Harry Met Sally | role: Sally Albright")
    }
}

void TH_inicializa(){
    FILE *f1 = fopen("hash_chaves.bin", "wb");
    FILE *f2 = fopen("hash_dados.bin", "wb");

    if(!f1 || !f2){
        printf("Não foi possível criar o arquivos da hash\n");
        exit(1);
    }

    fclose(f1);
    fclose(f2);
}

TH *TH_aloca(char *filme){
    TH *novo = (TH*) malloc(sizeof(TH));
    strcpy(novo->filme, filme);
    novo->val = 1;
    novo->end = -1;
    return novo;
}

TH *TH_busca_filme(char *filme){
    FILE *f = fopen("hash_chaves.bin", "rb");
    if(!f){
        printf("Não foi possivel abrir o arquivo hash_chaves.bin para busca\n");
        exit(1);
    }

    TH h;
    while(fread(&h, sizeof(TH), 1, f)){
        if(strcmp(h.filme, filme)){
            TH *temp = TH_aloca(filme);
            temp->end = h.end;
            fclose(f);
            return temp;
        }
    }
    fclose(f);
    return NULL;
}

int TH_busca(char *chave, char *elem){
    FILE *f1 = fopen("hash_chaves.bin", "rb");
    FILE *f2 = fopen("hash_dados.bin", "rb");

    if(!f1 || !f2){
        printf("Não foi possível abrir os arquivos da hash para busca\n");
        exit(1);
    }

    TH h;
    TR r;
    while(fread(&h, sizeof(TH), 1, f1)){
        if(strcmp(h.filme, chave) == 0){
            fseek(f2, h.end, SEEK_SET);
            int end = ftell(f2);
            int le = fread(&r, sizeof(TR), 1, f2);
            while(le){
                if(strcmp(r.nome, elem) == 0){
                    fclose(f1);
                    fclose(f2);
                    if(r.val == 1) return end;
                }
                if(r.prox == -1){
                    break;
                }
                fseek(f2, r.prox, SEEK_SET);
                end = ftell(f2);
                le = fread(&r, sizeof(TR), 1, f2);
            }
            fclose(f1);
            fclose(f2);
            return -1;
        }
    }
    fclose(f1);
    fclose(f2);
    return -1;
}

void TH_insere_elem(char *filme, TR *elem){
    int a = TH_busca(filme, elem.nome);
    if(a != -1) return;
    
    FILE *c = fopen("hash_chaves.bin", "rb+");
    FILE *r = fopen("hash_dados.bin", "rb+");
    if(!c || !r) exit(1);



    TH temp;
    TR aux;
    while(fread(&temp, sizeof(TH), 1, c) == 1){
        if(strcmp(temp.filme, filme) == 0){
            fseek(r, temp.end, SEEK_SET);
            fread(&aux, sizeof(TR), 1, r);
        }
    }
}