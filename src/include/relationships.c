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

void push_livre(int offset){
    FILE* f = fopen(arqPosLivre, "ab");
    if(!f) exit(1);
    fwrite(&offset, sizeof(int), 1, f);
    fclose(f);
}

int pop_livre(){
    FILE* flivres = fopen(arqPosLivre, "rb+");
    if(!flivres) return -1; 
    
    fseek(flivres, 0, SEEK_END);
    if(ftell(flivres) < sizeof(int)){ 
        fclose(flivres);
        return -1;
    }
    int offset;
    
    fseek(flivres, -sizeof(int), SEEK_END);
    fread(&offset, sizeof(int), 1, flivres);
    int pos = ftell(flivres);
    fclose(flivres);
    truncate(arqPosLivre, pos); 

    return offset;
}

void trim(char* str){

    int inicio = 0;
    while(str[inicio] == ' ') inicio++;
    

    int i = 0;
    while(str[inicio + i] != '\0'){
        str[i] = str[inicio + i];
        i++;
    }
    str[i] = '\0';

    int fim = strlen(str) - 1;
    while(fim >= 0 && str[fim] == ' ')
        str[fim--] = '\0';
}

void txt_to_hashMS(char *txt){
    FILE* fteste = fopen(arqDado, "rb");
    fseek(fteste, 0, SEEK_END);
    if(ftell(fteste) > sizeof(TR)) return; //Arquivo ja preenchido com Relationships.txt
    
    FILE *finput = fopen(txt, "r");
    if(!finput) exit(1);
    

    char linha[256];
    while(fgets(linha, sizeof(linha), finput)){
        //START Person | Meg Ryan | ACTED_IN | END Movie | When Harry Met Sally | role: Sally Albright
        linha[strcspn(linha, "\n")] = '\0';
        char* token = strtok(linha, "|"); // "START Person "
        if(!token) continue;

        token = strtok(NULL, "|"); // " Nome da Pessoa "
        if(!token) continue;
        trim(token);
        char nome[50];
        strncpy(nome, token, sizeof(nome) - 1);


        token = strtok(NULL, "|");// " TIPO_REL "
        if(!token) continue;
        trim(token);
        TipoRelacao tipo;
        if(strcmp(token, "ACTED_IN") == 0) tipo = ACTED_IN;
        else if(strcmp(token, "DIRECTED") == 0) tipo = DIRECTED;
        else if(strcmp(token, "PRODUCED") == 0) tipo = PRODUCED;
        else tipo = WROTE;

        strtok(NULL, "|");// " END Movie "
        if(!token) continue;

        token = strtok(NULL, "|");// " Nome Do Filme "
        if(!token) continue;
        trim(token);
        char filme[100];
        strncpy(filme, token, sizeof(filme) - 1);
        if(tipo == ACTED_IN){
            token = strtok(NULL, "|"); // " role: Papel 1, Papel 2, Papel 3"
            if(!token) continue;
            trim(token);
            char* dois_pontos = strchr(token, ':');
            if(!dois_pontos) continue;
            char* papeis = dois_pontos + 2;
            if(!papeis) continue;

            char* papel = strtok(papeis, ","); // " Papel 2 "
            if(!papel) continue;
            while(papel != NULL){
                if(!papel) continue;
                trim(papel);
                TR elem;
                elem.valido = 1;
                strcpy(elem.nome, nome);
                strcpy(elem.papel, papel);
                elem.tipo = tipo;
                elem.prox = -1;

                TR_insere(filme, &elem);
                papel = strtok(NULL, ",");
            }
        }
        else{
            TR elem;
            elem.valido = 1;
            elem.prox = -1;
            elem.tipo = tipo;
            elem.papel[0] = '\0';
            strcpy(elem.nome, nome);

            
            TR_insere(filme, &elem);
        }
    }
    fclose(finput);
}

void TH_inicializa(){
    FILE* teste = fopen(arqHash, "rb");
    if(teste){
        fclose(teste);
        return;
    }
    
    FILE* fhash = fopen(arqHash, "wb");
    FILE* fdados = fopen(arqDado, "wb");
    FILE* flivres = fopen(arqPosLivre, "wb");

    if((!fhash || !fdados)|| !flivres){
        printf("Não foi possível criar os arquivos da hash\n");
        exit(1);
    }
    fclose(fdados);
    fclose(flivres);

    int i;
    int vazio = -1;
    for(i=0; i<TAM_HASH; i++){
        fwrite(&vazio, sizeof(int), 1, fhash); //preenche arquivo criado com -1
    }
    fclose(fhash);
}

TH *TH_aloca(char *filme){
    TH *novo = (TH*) malloc(sizeof(TH));
    strcpy(novo->filme, filme);
    novo->valido = 1;
    novo->end = -1;
    return novo;
}

TH *TH_busca_filme(char *filme){
    FILE *f = fopen(arqHash, "rb");
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
    FILE *f1 = fopen(arqHash, "rb");
    FILE *f2 = fopen(arqDado, "rb");

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

void TR_insere(char* filme, TR* elem){
    int a = TR_busca_filme(filme);
    if(a == -1){                        //Filme nao existe na tabela
        FILE* fhash = fopen(arqHash, "rb+");
        FILE* fdados = fopen(arqDado, "rb+");
        if(!fhash || !fdados) exit(1);
        int offset_arqHash = hashit(filme) * sizeof(int);

        int novo_offset = pop_livre();
        if(novo_offset == -1){
            fseek(fdados, 0, SEEK_END);
            novo_offset = ftell(fdados);
        }
        fseek(fdados, novo_offset, SEEK_SET);
        fwrite(elem, sizeof(TR), 1, fdados);
        fseek(fhash, offset_arqHash, SEEK_SET);
        fwrite(&novo_offset, sizeof(int), 1, fhash);
        fclose(fhash);
        fclose(fdados);
        return;
    }

    int offset_elemento = TR_busca_endereco_por_papel(filme, elem->nome, elem->papel);
    if(offset_elemento != -1) return;
    FILE* fdados = fopen(arqDado, "rb+");
    if(!fdados) exit(1);

    TR atual;
    fseek(fdados, a, SEEK_SET);
    fread(&atual, sizeof(TR), 1, fdados);
    while(atual.prox != -1){
        a = atual.prox;
        fseek(fdados, a, SEEK_SET);
        fread(&atual, sizeof(TR), 1, fdados);
    }

    int novo_offset = pop_livre();
    if(novo_offset == -1){
        fseek(fdados, 0, SEEK_END);
        novo_offset = ftell(fdados);
    }
    atual.prox = novo_offset;
    fseek(fdados, novo_offset, SEEK_SET);
    fwrite(elem, sizeof(TR), 1, fdados);
    fseek(fdados, a, SEEK_SET);
    fwrite(&atual, sizeof(TR), 1, fdados);
    fclose(fdados);
}

int TR_busca_filme(char* filme){
    int offset_arqHash = hashit(filme) * sizeof(int);
    FILE* fhash = fopen(arqHash, "rb");
    if(!fhash) exit(1);
    
    int offset_dado;
    fseek(fhash, offset_arqHash, SEEK_SET);
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

    FILE* fdados = fopen(arqDado, "rb");
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

    FILE* fdados = fopen(arqDado, "rb");
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

    FILE* fdados = fopen(arqDado, "rb");
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

int TR_busca_anterior(FILE* fdados, char* filme, char* nome_pessoa){
    int offset = TR_busca_filme(filme);
    if(offset == -1) return -1;

    int offset_anterior = -1;
    TR aux;

    while(offset != -1){
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);

        if(strcasecmp(aux.nome, nome_pessoa) == 0 && aux.valido == 1){
            return offset_anterior;
        }
        offset_anterior = offset;
        offset = aux.prox;
    }
    return -1;
}

int TR_busca_anterior_por_papel(FILE* fdados, char* filme, char* nome_pessoa, char* papel){
    int offset = TR_busca_filme(filme);
    if(offset == -1) return -1;

    int offset_anterior = -1;
    TR aux;

    while(offset != -1){
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);

        if((strcasecmp(aux.nome, nome_pessoa) == 0 && aux.valido == 1) && strcasecmp(aux.papel, papel) == 0){
            return offset_anterior;
        }
        offset_anterior = offset;
        offset = aux.prox;
    }
    return -1;
}

void TR_retira_relacao_por_nome(char* filme, char* nome_pessoa){
    int offset_rel = TR_busca_endereco_elem(filme, nome_pessoa);
    if(offset_rel == -1) return;

    FILE* fdados = fopen(arqDado, "rb+");
    if(!fdados) exit(1);

    int offset_ant = TR_busca_anterior(fdados, filme, nome_pessoa);

    TR rel;
    fseek(fdados, offset_rel, SEEK_SET);
    fread(&rel, sizeof(TR), 1, fdados);

    if(offset_ant != -1){
        TR anterior;
        fseek(fdados, offset_ant, SEEK_SET);
        fread(&anterior, sizeof(TR), 1, fdados);
        anterior.prox = rel.prox;
        fseek(fdados, offset_ant, SEEK_SET);
        fwrite(&anterior, sizeof(TR), 1, fdados);
    }

    fseek(fdados, offset_rel, SEEK_SET);
    rel.valido = 0;
    fwrite(&rel, sizeof(TR), 1, fdados);
    push_livre(offset_rel);
    fclose(fdados);
}

void TR_retira_relacao_por_papel(char* filme, char* nome_pessoa, char* papel){
    int offset_rel = TR_busca_endereco_por_papel(filme, nome_pessoa, papel);
    if(offset_rel == -1) return;

    FILE* fdados = fopen(arqDado, "rb+");
    if(!fdados) exit(1);

    int offset_ant = TR_busca_anterior_por_papel(fdados, filme, nome_pessoa, papel);

    TR rel;
    fseek(fdados, offset_rel, SEEK_SET);
    fread(&rel, sizeof(TR), 1, fdados);

    if(offset_ant != -1){
        TR anterior;
        fseek(fdados, offset_ant, SEEK_SET);
        fread(&anterior, sizeof(TR), 1, fdados);
        anterior.prox = rel.prox;
        fseek(fdados, offset_ant, SEEK_SET);
        fwrite(&anterior, sizeof(TR), 1, fdados);
    }

    fseek(fdados, offset_rel, SEEK_SET);
    rel.valido = 0;
    fwrite(&rel, sizeof(TR), 1, fdados);
    push_livre(offset_rel);
    fclose(fdados);
}

void TR_retira_tudo(char* filme){
    int hash_offset = hashit(filme) * sizeof(int);
    FILE* fhash = fopen(arqHash, "rb+");
    if(!fhash) exit(1);
    int data_offset;
    fseek(fhash, hash_offset, SEEK_SET);
    fread(&data_offset, sizeof(int), 1, fhash);
    FILE* fdata = fopen(arqDado, "rb+");
    if(!fdata) exit(1);
    TR rel;
    while(data_offset != -1){
        fseek(fdata, data_offset, SEEK_SET);
        fread(&rel, sizeof(TR), 1, fdata);
        if(rel.valido == 1) {
            fseek(fdata, data_offset, SEEK_SET);
            rel.valido = 0;
            fwrite(&rel, sizeof(TR), 1, fdata);
            push_livre(data_offset);
        }
        data_offset = rel.prox;
    }
    fseek(fhash, hash_offset, SEEK_SET);
    int vazio = -1;
    fwrite(&vazio, sizeof(int), 1, fhash);
    fclose(fhash);
    fclose(fdata);
}

void TR_imprime_relacoes_filme_pessoa(char* filme, char* pessoa){
    int offset = TR_busca_filme(filme);
    if(offset == -1){
        printf("Filme não encontrado!\n");
        return;
    }

    FILE* fdados = fopen(arqDado, "rb");
    if(!fdados) exit(1);

    int encontrou = 0;
    TR aux;
    while(offset != -1){
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        if(strcasecmp(aux.nome, pessoa) == 0 && aux.valido == 1){
            encontrou = 1;
            printf("Nome: %s | Tipo: %d | Papel: %s\n", aux.nome, aux.tipo, aux.papel);
        }
        offset = aux.prox;
    }
    if(!encontrou) printf("Nenhum relacionamento encontrado!\n");
    fclose(fdados);
}

void TR_imprime_relacoes_filme(char* filme){

    int offset = TR_busca_filme(filme);

    if(offset == -1){
        printf("Filme não encontrado!\n");
        return;
    }

    FILE* fdados = fopen(arqDado, "rb");

    if(!fdados) exit(1);

    printf("=== %s ===\n", filme);
    TR aux;
    while(offset != -1){
        
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);

        if(aux.valido == 1){
            char* tipo_str;
            switch(aux.tipo){
                case ACTED_IN:  tipo_str = "ACTED_IN";  break;
                case DIRECTED:  tipo_str = "DIRECTED";  break;
                case PRODUCED:  tipo_str = "PRODUCED";  break;
                case WROTE:     tipo_str = "WROTE";     break;
                default:        tipo_str = "?";         break;
            }
            printf("  %s | %s | %s\n", aux.nome, tipo_str, aux.papel);
        }
        offset = aux.prox;
    }
    fclose(fdados);
}

void menu(){
    int opcao;
    char filme[100];
    char nome[50];
    char papel[50];

    do {
        printf("\n=== MENU ===\n");
        printf("1. Inserir relacionamento\n");
        printf("2. Buscar relacionamentos de um filme\n");
        printf("3. Buscar relacionamentos de uma pessoa num filme\n");
        printf("4. Remover relacionamento por nome\n");
        printf("5. Remover relacionamento por papel\n");
        printf("6. Remover todos os relacionamentos de um filme\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);
        getchar(); // consome o \n deixado pelo scanf

        switch(opcao){
            case 1:
                printf("Filme: ");
                fgets(filme, sizeof(filme), stdin);
                filme[strcspn(filme, "\n")] = '\0';

                printf("Nome da pessoa: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';

                printf("Tipo (0=ACTED_IN, 1=DIRECTED, 2=PRODUCED, 3=WROTE): ");
                int tipo;
                scanf("%d", &tipo);
                getchar();

                printf("Papel (enter para vazio): ");
                fgets(papel, sizeof(papel), stdin);
                papel[strcspn(papel, "\n")] = '\0';

                TR elem;
                elem.valido = 1;
                elem.prox = -1;
                elem.tipo = tipo;
                strcpy(elem.nome, nome);
                strcpy(elem.papel, papel);
                TR_insere(filme, &elem);
                printf("Inserido com sucesso!\n");
                break;

            case 2:
                printf("Filme: ");
                fgets(filme, sizeof(filme), stdin);
                filme[strcspn(filme, "\n")] = '\0';
                TR_imprime_relacoes_filme(filme);
                break;

            case 3:
                printf("Filme: ");
                fgets(filme, sizeof(filme), stdin);
                filme[strcspn(filme, "\n")] = '\0';

                printf("Nome da pessoa: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';
                TR_imprime_relacoes_filme_pessoa(filme, nome);
                break;

            case 4:
                printf("Filme: ");
                fgets(filme, sizeof(filme), stdin);
                filme[strcspn(filme, "\n")] = '\0';

                printf("Nome da pessoa: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';
                TR_retira_relacao_por_nome(filme, nome);
                printf("Removido com sucesso!\n");
                break;

            case 5:
                printf("Filme: ");
                fgets(filme, sizeof(filme), stdin);
                filme[strcspn(filme, "\n")] = '\0';

                printf("Nome da pessoa: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';

                printf("Papel: ");
                fgets(papel, sizeof(papel), stdin);
                papel[strcspn(papel, "\n")] = '\0';
                TR_retira_relacao_por_papel(filme, nome, papel);
                printf("Removido com sucesso!\n");
                break;

            case 6:
                printf("Filme: ");
                fgets(filme, sizeof(filme), stdin);
                filme[strcspn(filme, "\n")] = '\0';
                TR_retira_tudo(filme);
                printf("Todos os relacionamentos removidos!\n");
                break;

            case 0:
                printf("Saindo...\n");
                break;

            default:
                printf("Opcao invalida!\n");
                break;
        }
    } while(opcao != 0);
}