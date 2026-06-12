#include <string.h>
#include "../header/util.h"
#include "../header/TARVBMS.h"

int compara_entidades_por_nome(TD *e1, TD *e2)
{
    char nome1[100];
    char nome2[100];

    if (e1->tipo == TIPO_PESSOA)
    {
        strcpy(nome1, e1->dados.pessoa.nome);
    }
    else
    {
        strcpy(nome1, e1->dados.filme.titulo);
    }

    if (e2->tipo == TIPO_PESSOA)
    {
        strcpy(nome2, e2->dados.pessoa.nome);
    }
    else
    {
        strcpy(nome2, e2->dados.filme.titulo);
    }

    return strcmp(nome1, nome2);
}

int compara_nomes_com_entidades(char *nome, TD *entidade){
    char nome2[100];

    if(entidade->tipo == TIPO_PESSOA){
        strcpy(nome2, entidade->dados.pessoa.nome);
    }
    else{
        strcpy(nome2, entidade->dados.filme.titulo);
    }

    return strcmp(nome, nome2);
}

void le_no_interno(FILE *f, long posicao, TARVBMSDADOS *infos,
                   char (*ids)[TAM_NOME], long *filhos, char (*arq_folha)[TAM_ARQ_FOLHA])
{
    TConfig cfg;
    rewind(f);
    fread(&cfg, sizeof(TConfig), 1, f);
    int max_chaves = (2 * cfg.t) - 1;

    fseek(f, posicao, SEEK_SET);
    fread(infos, sizeof(TARVBMSDADOS), 1, f);
    fread(ids, TAM_NOME, max_chaves, f);
    fread(arq_folha, TAM_ARQ_FOLHA, max_chaves + 1, f);
    fread(filhos, sizeof(long), max_chaves + 1, f);
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