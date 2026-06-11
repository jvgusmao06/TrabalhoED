#include "../header/TD.h"
#include "../header/TARVBMS.h"

TFOLHA *TFOLHA_aloca(int t)
{
    TFOLHA *novo = (TFOLHA *)malloc(sizeof(TFOLHA));
    int tam = (t * 2) - 1;
    novo->entidades = (TD *)malloc(sizeof(TD) * tam);
    novo->dados.nchaves = 0;
    return novo;
}

TFOLHA *TFOLHA_le(int indice_folha, int t)
{
    char arq[30];
    sprintf(arq, "../dados/folha_%d.bin", indice_folha);

    FILE *f = fopen(arq, "rb");
    if (!f)
    {
        printf("Erro ao abrir folha_%d.bin. Funcao TARVBMS_le_folha\n", indice_folha);
        return NULL;
    }

    TFOLHA *folha = TARVBMS_aloca_folha(t);
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
        exit(1);
    }

    TConfig c;
    fread(&c, sizeof(TConfig), 1, f);

    if (c.indice <= 0 || c.raiz == -1)
    {
        fclose(f);
        return NULL;
    }

    if (c.indice == 1) // A árvore tem apenas uma folha e ela é a raíz
    {
        fclose(f);
        FILE *f = fopen("../dados/folha_0.bin", "rb");
        TFOLHA *folha = TARVBMS_aloca_folha(c.t);
        fread(&folha->dados, sizeof(TDADOSFOLHA), 1, f);
        fread(folha->entidades, sizeof(TD), folha->dados.nchaves, f);

        for (int i = 0; i < folha->dados.nchaves; i++)
        {
            if (folha->entidades[i].tipo == TIPO_PESSOA)
            {
                if (strcmp(nome, folha->entidades[i].dados.pessoa.nome) == 0)
                {
                    fclose(f);
                    return folha;
                }
            }
            else
            {
                if (strcmp(nome, folha->entidades[i].dados.filme.titulo) == 0)
                {
                    fclose(f);
                    return folha;
                }
            }
        }
        free(folha->entidades);
        free(folha);
        fclose(f);
        return NULL;
    }
    // Tem um arquivo de índice
    long pos_atual = c.raiz;
    TARVBMSDADOS no_infos;
    int max_chaves = (c.t * 2) - 1;
    char ids[max_chaves][100];
    char arq_folha[max_chaves + 1][30];
    long filhos[max_chaves + 1];
    while (1)
    {
        TARVBMS_le_no_interno(f, pos_atual, &no_infos, ids, filhos, arq_folha);

        if (no_infos.nchaves <= 0 || no_infos.nchaves > max_chaves)
        {
            fclose(f);
            return NULL;
        }

        int i = 0;
        while (i < no_infos.nchaves && strcmp(nome, ids[i]) > 0)
            i++;

        if (no_infos.val == 1)
        {
            char nome_arquivo_folha[30];
            strcpy(nome_arquivo_folha, arq_folha[i]);

            fclose(f);

            char caminho_folha[60];
            sprintf(caminho_folha, "../dados/%s", nome_arquivo_folha);

            FILE *ffolha = fopen(caminho_folha, "rb");
            if (!ffolha)
                return NULL;

            TFOLHA *folha = TARVBMS_aloca_folha(c.t);
            fread(&folha->dados, sizeof(TDADOSFOLHA), 1, ffolha);
            fread(folha->entidades, sizeof(TD), folha->dados.nchaves, ffolha);

            fclose(ffolha);
            return folha;
        }
        else
        {
            pos_atual = filhos[i];
        }
    }
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

        folha = TARVBMS_aloca_folha(t);
        
        folha->dados.nchaves += 1;
        folha->entidades[0] = *entidade;

        TARVBMS_cria_folha(folha, t, index);

        free(folha->entidades);
        free(folha);
        return;
    }

    folha = TARVBMS_aloca_folha(t);
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
    while (i >= 0 && comparar_por_nome(&folha->entidades[i], entidade) > 0)
    {
        folha->entidades[i + 1] = folha->entidades[i];
        i--;
    }

    folha->entidades[i + 1] = *entidade;
    folha->dados.nchaves++;

    TARVBMS_cria_folha(folha, t, index);
    free(folha->entidades);
    free(folha);
}
