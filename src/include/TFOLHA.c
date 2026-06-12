#include <stdlib.h>
#include <string.h>
#include "../header/util.h"
#include "../header/TD.h"
#include "../header/TFOLHA.h"
#include "../header/TARVBMS.h"

TFOLHA *TFOLHA_aloca(int t)
{
    TFOLHA *novo = (TFOLHA *)malloc(sizeof(TFOLHA));
    int tam = (t * 2) - 1;
    novo->entidades = (TD *)malloc(sizeof(TD) * tam);
    novo->dados.nchaves = 0;
    novo->dados.prox[0] = '\0';
    return novo;
}

TFOLHA *TFOLHA_le(int indice_folha, int t)
{
    char arq[30];
    sprintf(arq, "../dados/folha_%d.bin", indice_folha);

    FILE *f = fopen(arq, "rb");
    if (!f)
    {
        printf("Erro ao abrir folha_%d.bin. Funcao TFolha_le\n", indice_folha);
        return NULL;
    }

    TFOLHA *folha = TFOLHA_aloca(t);
    fread(&folha->dados, sizeof(TDADOSFOLHA), 1, f);
    fread(folha->entidades, sizeof(TD), folha->dados.nchaves, f);

    fclose(f);
    return folha;
}

void TFOLHA_cria(TFOLHA *folha, int t, int index)
{ // Cria o arquivo das folhas
    char arq[30];
    sprintf(arq, "../dados/folha_%d.bin", index);

    FILE *f = fopen(arq, "wb");
    if (!f)
    {
        printf("Não foi possível criar o arquivo folha %d\n", index);
        exit(1);
    }

    int tam = (t * 2) - 1;
    fwrite(&folha->dados, sizeof(TDADOSFOLHA), 1, f);
    fwrite(folha->entidades, sizeof(TD), folha->dados.nchaves, f);
    fclose(f);
}

void TFOLHA_imprime(TFOLHA *folha)
{
    if (!folha)
        return;

    printf("n chave = %d\n", folha->dados.nchaves);
    printf("prox = %s\n", folha->dados.prox);
    for (int i = 0; i < folha->dados.nchaves; i++)
    {
        if (folha->entidades[i].tipo == TIPO_PESSOA)
            printf("folha [%d] = %s\n", i, folha->entidades[i].dados.pessoa.nome);
        else
            printf("folha [%d] = %s\n", i, folha->entidades[i].dados.filme.titulo);
    }
}

TFOLHA *TFOLHA_carrega(char *nome)
{
    FILE *f = fopen("../dados/index.bin", "rb");
    if (!f)
    {
        printf("Erro ao abrir o arquivo ../dados/index.bin para busca\n");
        return NULL;
    }

    TConfig c;
    fread(&c, sizeof(TConfig), 1, f);

    if (c.indice <= 0)
    {
        fclose(f);
        return NULL;
    }

    int indice_folha;

    if (c.raiz == -1)
    {
        /* Sem raiz — a árvore só tem a folha_0 */
        indice_folha = 0;
    }
    else
    {
        /* Desce pela árvore até encontrar a folha correta */
        int max_chaves = (c.t * 2) - 1;
        long pos_atual = c.raiz;
        TARVBMSDADOS no_infos;
        char ids[max_chaves][TAM_NOME];
        char arq_folha[max_chaves + 1][TAM_ARQ_FOLHA];
        long filhos[max_chaves + 1];

        while (1)
        {
            le_no_interno(f, pos_atual, &no_infos, ids, filhos, arq_folha);

            int i = 0;
            while (i < no_infos.nchaves && strcmp(nome, ids[i]) >= 0)
                i++;

            if (no_infos.filho_folha)
            {
                sscanf(arq_folha[i], "folha_%d.bin", &indice_folha);
                break;
            }
            else
            {
                pos_atual = filhos[i];
            }
        }
    }

    fclose(f);

    /* Lê a folha e procura a chave */
    TFOLHA *folha = TFOLHA_le(indice_folha, c.t);
    if (!folha) return NULL;

    for (int i = 0; i < folha->dados.nchaves; i++)
    {
        char chave[TAM_NOME];
        if (folha->entidades[i].tipo == TIPO_PESSOA)
            strcpy(chave, folha->entidades[i].dados.pessoa.nome);
        else
            strcpy(chave, folha->entidades[i].dados.filme.titulo);

        if (strcmp(chave, nome) == 0)
            return folha; /* Encontrado! */
    }

    /* Não encontrado */
    free(folha->entidades);
    free(folha);
    return NULL;
}

void TFOLHA_insere(TD *entidade, int t, int index)
{ // Insere ordenado um TD na folha caso não esteja cheia
    char arq[30];
    sprintf(arq, "../dados/folha_%d.bin", index);

    FILE *f = fopen(arq, "rb+");

    TFOLHA *folha = NULL;

    if (!f)
    {
        printf("A folha ../dados/folha_%d.bin não existe. Criando uma folha.\n", index);

        folha = TFOLHA_aloca(t);
        
        folha->entidades[0] = *entidade;

        TFOLHA_cria(folha, t, index);

        free(folha->entidades);
        free(folha);
        return;
    }

    folha = TFOLHA_aloca(t);
    fread(&(folha->dados), sizeof(TDADOSFOLHA), 1, f);

    fread(folha->entidades, sizeof(TD), folha->dados.nchaves, f);
    fclose(f);

    int tam_max = (t * 2) - 1;
    if (folha->dados.nchaves >= tam_max)
    {
        printf("Erro: A folha %d está cheia.\n", index);
        free(folha->entidades);
        free(folha);
        return;
    }

    int i = folha->dados.nchaves - 1;
    while (i >= 0 && compara_entidades_por_nome(&folha->entidades[i], entidade) > 0)
    {
        folha->entidades[i + 1] = folha->entidades[i];
        i--;
    }

    folha->entidades[i + 1] = *entidade;
    folha->dados.nchaves++;

    TFOLHA_cria(folha, t, index);
    free(folha->entidades);
    free(folha);
}