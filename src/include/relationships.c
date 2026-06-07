#include "../header/relationships.h"
#include <string.h>

int hashit(char* nome){
    char copia[100];
    strncpy(copia, nome, sizeof(copia) - 1);
    copia[99] = '\0';

    char* str = copia;
    for(int i = 0; str[i] != '\0'; i++)
        str[i] = tolower(str[i]);

    unsigned int hash = 5381;
    int c;
    while((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % TAM_HASH;
}


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
    FILE *f1 = fopen(THash, "wb");
    FILE *f2 = fopen(THDado, "wb");

    if(!f1 || !f2){
        printf("Não foi possível criar o arquivos da hash\n");
        exit(1);
    }
    fclose(f2);

    int i;
    int vazio = -1;
    for(i=0; i<TAM_HASH; i++){
        fwrite(&vazio, sizeof(int), 1, f1); //preenche arquivo criado com -1
    }
    fclose(f1);
}


TH *TH_aloca(char *filme){
    TH *novo = (TH*) malloc(sizeof(TH));
    strcpy(novo->filme, filme);
    novo->valido = 1;
    novo->end = -1;
    return novo;
}


TH *TH_busca_filme(char *filme){
    FILE *f = fopen(THash, "rb");
    if(!f){
        printf("Não foi possivel abrir o arquivo hash_chaves.bin para busca\n");
        exit(1);
    }

    TH h;
    while(fread(&h, sizeof(TH), 1, f)){
        if(strcasecmp(h.filme, filme) == 0){
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
    FILE *f1 = fopen(THash, "rb");
    FILE *f2 = fopen(THDado, "rb");

    if(!f1 || !f2){
        printf("Não foi possível abrir os arquivos da hash para busca\n");
        exit(1);
    }

    TH h;
    TR r;
    while(fread(&h, sizeof(TH), 1, f1)){
        if(strcasecmp(h.filme, chave) == 0){
            fseek(f2, h.end, SEEK_SET);
            int end = ftell(f2);
            int le = fread(&r, sizeof(TR), 1, f2);
            while(le){
                if(strcasecmp(r.nome, elem) == 0){
                    fclose(f1);
                    fclose(f2);
                    if(r.valido == 1) return end;
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


void TH_insere(char* filme, TR* elem){
    int a = TR_busca_filme(filme);
    if(a == -1){                        //Filme nao existe na tabela
        FILE* fhash = fopen(THash, "rb+");
        FILE* fdados = fopen(THDado, "rb+");
        if(!fhash || !fdados) exit(1);
        int offset_thash = hashit(filme) * sizeof(int);
        int novo_offset;
        fseek(fdados, 0, SEEK_END);
        novo_offset = ftell(fdados);
        fwrite(elem, sizeof(TR), 1, fdados);
        fseek(fhash, offset_thash, SEEK_SET);
        fwrite(&novo_offset, sizeof(int), 1, fhash);
        fclose(fhash);
        fclose(fdados);
        return;
    }

    int offset_elemento = TR_busca_endereco_elem(filme, elem->nome);
    if(offset_elemento != -1) return;
    FILE* fdados = fopen(THDado, "rb+");
    if(!fdados) exit(1);

    TR atual;
    fseek(fdados, a, SEEK_SET);
    fread(&atual, sizeof(TR), 1, fdados);
    while(atual.prox != -1){
        a = atual.prox;
        fseek(fdados, a, SEEK_SET);
        fread(&atual, sizeof(TR), 1, fdados);
    }
    fseek(fdados, 0, SEEK_END);
    atual.prox = ftell(fdados);
    fwrite(elem, sizeof(TR), 1, fdados);
    fseek(fdados, a, SEEK_SET);
    fwrite(&atual, sizeof(TR), 1, fdados);
    fclose(fdados);
}


int TR_busca_filme(char* nome){
    int offset_thash = hashit(nome) * sizeof(int);
    FILE* fhash = fopen(THash, "rb");
    if(!fhash) exit(1);
    
    int offset_dado;
    fseek(fhash, offset_thash, SEEK_SET);
    fread(&offset_dado, sizeof(int), 1, fhash);
    fclose(fhash);
    if(offset_dado == -1){
        printf("Filme não encontrado!\n");
        return -1;
    }
    return offset_dado;
}


TR* TR_busca_elem(char* filme, char* pessoa){
    int offset = TR_busca_filme(filme);
    if(offset == -1) return NULL;

    FILE* fdados = fopen(THDado, "rb");
    if(!fdados) exit(1);

    
    TR aux;
    while(offset != -1){
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        if(strcasecmp(aux.nome, pessoa) == 0){
            if(aux.valido == 1){
                TR* resp = (TR*) malloc(sizeof(TR));
                strcpy(resp->nome, aux.nome);
                resp->tipo = aux.tipo;
                strcpy(resp->papel, aux.papel);
                resp->prox = aux.prox; 
                fclose(fdados);
                return resp;
            }
            fclose(fdados);
            return NULL;
        }
        offset = aux.prox;
    }
    fclose(fdados);
    return NULL;
}

int TR_busca_endereco_elem(char* filme, char* pessoa){
    int offset = TR_busca_filme(filme);
    if(offset == -1) return -1;

    FILE* fdados = fopen(THDado, "rb");
    if(!fdados) exit(1);

    
    TR aux;
    while(offset != -1){
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        if(strcasecmp(aux.nome, pessoa) == 0){
            if(aux.valido == 1){
                fclose(fdados);
                return offset;
            }
            fclose(fdados);
            return -1;
        }
        offset = aux.prox;
    }
    fclose(fdados);
    return -1;
}

int TR_busca_endereco_por_papel(char* filme, char* nome_pessoa, char* papel){
    int offset = TR_busca_filme(filme);
    if(offset == -1) return -1;

    FILE* fdados = fopen(THDado, "rb");
    if(!fdados) exit(1);

    
    TR aux;
    while(offset != -1){
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        if(strcasecmp(aux.nome, nome_pessoa) == 0){
            if(strcasecmp(aux.papel, papel) == 0){
                if(aux.valido == 1){
                    fclose(fdados);
                    return offset;
                }
                fclose(fdados);
                return -1;
            }
        }
        offset = aux.prox;
    }
    fclose(fdados);
    return -1;
}

void TR_retira_relacao_por_nome(char* filme, char* nome_pessoa){
    int offset_rel = TR_busca_endereco_elem(filme, nome_pessoa);
    if(offset_rel == -1) return;
    FILE* fdados = fopen(THDado, "rb+");
    TR aux;
    fseek(fdados, offset_rel, SEEK_SET);
    fread(&aux, sizeof(TR), 1, fdados);
    fseek(fdados, offset_rel, SEEK_SET);
    aux.valido = 0;
    fwrite(&aux, sizeof(TR), 1, fdados);
    fclose(fdados);
}

void TR_retira_relacao_por_papel(char* filme, char* nome_pessoa, char* papel){
    int offset_rel = TR_busca_endereco_por_papel(filme, nome_pessoa, papel);
    if(offset_rel == -1) return;
    FILE* fdados = fopen(THDado, "rb+");
    TR aux;
    fseek(fdados, offset_rel, SEEK_SET);
    fread(&aux, sizeof(TR), 1, fdados);
    fseek(fdados, offset_rel, SEEK_SET);
    aux.valido = 0;
    fwrite(&aux, sizeof(TR), 1, fdados);
    fclose(fdados);
}