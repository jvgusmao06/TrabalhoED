#include "../header/TD.h"
#include "../header/TARVBMS.h"

TFOLHA *TFOLHA_carrega(char *nome){
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