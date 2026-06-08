#include "../header/TARVBMS.h"
#include <stdlib.h>
#include <string.h>

TFOLHA *TARVBMS_aloca_folha(int t){
    TFOLHA *novo = (TFOLHA*) malloc(sizeof(TFOLHA));
    int tam = (t * 2) - 1;
    novo->entidades = (TD*) malloc(sizeof(TD) * tam);
    novo->dados.nchaves = 0;
    return novo;
}

void TARVBMS_cria_folha(TFOLHA folha, int t, int index){ // Cria o arquivo das folhas
    char arq[30];
    sprintf(arq, "../../dados/folha_%d.bin", index);

    FILE *f = fopen(arq, "wb");
    if(!f){
        printf("Não foi possível criar o arquivo folha %d\n", index);
        exit(1);
    }

    int tam = (t * 2) - 1;
    fwrite(&folha.dados, sizeof(TDADOSFOLHA), 1, f);
    fwrite(&folha.entidades, sizeof(TD), folha.dados.nchaves, f);
}

int comparar_por_nome(TD *e1, TD *e2){
    char nome1[100];
    char nome2[100];

    if(e1->tipo == TIPO_PESSOA){
        strcpy(nome1, e1->dados.pessoa.nome);
    }
    else{
        strcpy(nome1, e1->dados.filme.titulo);
    }

    if(e2->tipo == TIPO_PESSOA){
        strcpy(nome2, e2->dados.pessoa.nome);
    }
    else{
        strcpy(nome2, e2->dados.filme.titulo);
    }

    return strcmp(nome1, nome2);
}

void TARVBMS_insere_folha(TD *entidade, int t, int index){
    char arq[30];
    sprintf(arq, "../../dados/folha_%d.bin", index);

    FILE *f = fopen(arq, "rb+");
    if(!f){
        printf("Erro ao abrir o arquivo da folha %d para inserção.\n", index);
        exit(1);
    }

    TFOLHA *folha = TARVBMS_aloca_folha(t);
    fread(&(folha->dados), sizeof(TDADOSFOLHA), 1, f);
    
    fread(folha->entidades, sizeof(TD), folha->dados.nchaves, f);
    
    int tam_max = (t *2) - 1;
    if(folha->dados.nchaves >= tam_max){
        printf("Erro: A folha %d está cheia.\n", index);
        free(folha->entidades);
        free(folha);
        fclose(f);
        return;
    }

    int i = folha->dados.nchaves - 1;
    while(i >= 0 && comparar_por_nome(&folha->entidades[i], entidade) > 0){
        folha->entidades[i + 1] = folha->entidades[i];
        i--;
    }

    folha->entidades[i + 1] = *entidade;
    folha->dados.nchaves++;
    
    fclose(f);
    TARVBMS_cria_folha(*folha, t, index);
    free(folha->entidades);
    free(folha);
}

/*
void TARVBMS_ordena_folha(int index){
    char arq[30];
    sprintf(arq, "../../dados/folha_%d.bin", index);

    FILE *f = fopen(arq, "rb+");
    if(!f){
        printf("Não foi possível abrir o arquivo folha_%d.bin para ordenação", index);
        exit(1);
    }

    TDADOSFOLHA dados;
    fread(&dados, sizeof(TDADOSFOLHA), 1, f);

    if(dados.nchaves <= 1){
        fclose
    }
    int tam_max = sizeof(TDADOSFOLHA) + dados.nchaves * sizeof(TD);
    int i;
    TD ent;
    while(i< tam_max){

    }
}
*/

void TARVBMS_inicializa(int t){ // Cria todos os arquivos vazios
    FILE *f = fopen("../../dados/index.bin", "wb");

    if(!f){
        printf("Falha ao criar arquivo index. Funcao TARVBMS inicializa\n");
        exit(1);
    }
    TConfig temp;
    temp.raiz = -1;
    temp.t = t;
    temp.indice = 0;
    fwrite(&temp, sizeof(TConfig), 1, f);
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

void TARVBMS_insere_um_do_arquivotxt(char *nome)
{

    if(TARVBMS_busca(nome)){
        printf("Essa entidade ja esta na arvore!\n");
        return;
    }
    FILE *fn = fopen("../../entrada/Nodes.txt", "r");
    if(!fn){
        printf("Erro ao ler o arquivo nodes!\n");
        exit(1);
    }
    TD dado;
    int achou = 0;
    while(ler_entidade(fn, &dado)){
        if(dado.tipo == TIPO_FILME){
            if(!strcmp(dado.dados.filme.titulo, nome)){
            achou = 1;
            break;
        }

        }else if(dado.tipo == TIPO_PESSOA){
            if(!strcmp(dado.dados.pessoa.nome, nome)){
            achou = 1;
            break;
            }
        }
    }
    fclose(fn);
    if(!achou){
        printf("Nao existe essa entidade no nodes.txt!");
        return;
    }
    TARVBMS_insere_um(&dado);
    
    }






void TARVBMS_insere_um(TD *entidade, int t){
    if(!entidade){
        printf("Nao ha entidade! Funcao insere_um TARBMS");
        return;
    }
    FILE *f = fopen("../../dados/index.bin", "rb+");
    TConfig config;
    fseek(f, 0, SEEK_SET);
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;
    int max_chaves = (2*t)-1;
    
    char chave[100];
    if(entidade->tipo == TIPO_FILME){
        strcpy(chave, entidade->dados.filme.titulo);
    } else if(entidade->tipo == TIPO_PESSOA){
        strcpy(chave, entidade->dados.pessoa.nome);
    }

    
    if(config.raiz == -1){ // Avore esta vazia
        // Cria a primeira folha em memória principal e insere a entidade
        TFOLHA *nova_folha = TARVBMS_aloca_folha(t);
        nova_folha->entidades[0] = *entidade;
        nova_folha->dados.nchaves = 1;
        
        // Grava a folha no seu arquivo binário distinto (folha_0.bin) 
        TARVBMS_cria_folha(*nova_folha, t, config.indice);
        TARVBMSDADOS r_dados;
        r_dados.nchaves = 0;
        r_dados.filho_folha = 1;
        r_dados.val = 1;
        
        fseek(f, 0, SEEK_END);
        long pos_raiz = ftell(f);
        fwrite(&r_dados, sizeof(TARVBMSDADOS), 1, f);
        char nome_folha[30];
        sprintf(nome_folha, "../../dados/folha_%d.bin", config.indice);
        fwrite(nome_folha, sizeof(char) * 30, 1, f);
        
        config.raiz = pos_raiz;
        config.indice++;
        fseek(f, 0, SEEK_SET);
        fwrite(&config, sizeof(TConfig), 1, f);
        free(nova_folha->entidades);
        free(nova_folha);
    } 
}

TARVBMS *TARVBMS_aloca_no_interno(int t, TARVBMSDADOS *dados){
    TARVBMS *novo = (TARVBMS*)malloc(sizeof(TARVBMS));
    int max_chaves = (t * 2) - 1;
    novo->ids = (char**)malloc(sizeof(char[100]) * max_chaves);
    novo->infos.nchaves = 0;
    novo->infos.filho_folha = 1;
    novo->infos.val = 1;
    return novo;
}

void le_no_interno(FILE *f, long *posicao, TARVBMSDADOS *infos, char (*ids)[100], long *filhos, char (*arq_folha)[30]) {
    fseek(f, *posicao, SEEK_SET);
    
    fread(infos, sizeof(TARVBMSDADOS), 1, f);
    
    fread(ids, sizeof(char) * 100, infos->nchaves, f);
    
    if (infos->val == 1) {
        fread(arq_folha, sizeof(char) * 30, infos->nchaves + 1, f);
    } 
    else {
        fread(filhos, sizeof(long), infos->nchaves + 1, f);
    }
    *posicao = ftell(f);
}

char *TARVBMS_busca_no_internos(char *nome, int t) {
    FILE *f = fopen("../../dados/index.bin", "rb");
    if (!f) {
        printf("Erro ao abrir o arquivo ../../dados/index.bin para busca\n");
        exit(1);
    }

    TConfig c;
    fread(&c, sizeof(TConfig), 1, f);

    // Se a árvore estiver totalmente vazia (raiz inválida/0 ou menor)
    if (c.raiz <= 0) {
        fclose(f);
        return NULL; 
    }

    long pos_atual = c.raiz;
    TARVBMSDADOS no_infos;
    
    // Aloca matrizes/vetores auxiliares na RAM para ler o nó interno atual baseado no 't' max
    
    int max_chaves = (t * 2) - 1;
    char ids[max_chaves][100];
    char arq_folha[max_chaves + 1][30];
    long filhos[max_chaves + 1];

    while (1) {
        ler_no_interno(f, pos_atual, &no_infos, ids, filhos, arq_folha);

        int i = 0;
        while (i < no_infos.nchaves && strcmp(nome, ids[i]) > 0) {
            i++;
        }


        if (no_infos.val == 1) {
            char nome_arquivo_folha[30];
            strcpy(nome_arquivo_folha, arq_folha[i]);
            
            fclose(f);

            char caminho_folha[60];
            sprintf(caminho_folha, "../../dados/%s", nome_arquivo_folha);

            FILE *ffolha = fopen(caminho_folha, "rb");
            if (!ffolha) {
                return NULL;
            }

            TFOLHA *folha = TARVBMS_aloca_folha(t);
            fread(&(folha->dados), sizeof(TDADOSFOLHA), 1, ffolha);
            fread(folha->entidades, sizeof(TD), folha->dados.nchaves, ffolha);
            
            fclose(ffolha);
            return folha;
        } 
        else {
            pos_atual = filhos[i];
        }
    }
}
