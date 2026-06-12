#include <stdlib.h>
#include <string.h>
#include "../header/TARVBMS.h"
#include "../header/TFOLHA.h"
#include "../header/util.h"

void TARVBMS_imprime_folhas()
{
    FILE *f = fopen("../dados/index.bin", "rb+");
    if (!f)
    {
        printf("Erro ao abrir arquivo para imprime_folhas");
        exit(1);
    }

    TConfig config;
    fseek(f, 0, SEEK_SET);
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;
    int tam_max = (2 * t) - 1;

    long pos = config.raiz;
    TARVBMSDADOS infos;
    char ids[tam_max][TAM_NOME];
    long filhos[tam_max + 1];
    char arq_folha[tam_max + 1][TAM_ARQ_FOLHA];

    while (1)
    {
        le_no_interno(f, pos, &infos, ids, filhos, arq_folha);
        if (infos.filho_folha)
            break;
        pos = filhos[0];
    }

    for (int i = 0; i <= infos.nchaves; i++)
    {
        int indice;
        sscanf(arq_folha[i], "folha_%d.bin", &indice);

        TFOLHA *folha = TFOLHA_le(indice, t);
        if (!folha)
        {
            printf("Erro ao ler folha %s\n", arq_folha[i]);
            exit(1);
        }

        printf("--- Folha %s ---\n", arq_folha[i]);
        TFOLHA_imprime(folha);
        printf("\n");

        free(folha->entidades);
        free(folha);
    }

    fclose(f);
}

void TARVBMS_inicializa(int t) // Cria todos os arquivos vazios
{
    printf("Inicializando indice...\n");
    FILE *f = fopen("../dados/index.bin", "wb");

    if (!f)
    {
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

long TARVBMS_busca_index_invalido(FILE *f)
{
    rewind(f);
    TConfig c;
    fread(&c, sizeof(TConfig), 1, f);

    int max_chaves = (c.t * 2) - 1;

    int tam_no = sizeof(TARVBMSDADOS) + max_chaves * TAM_NOME + (max_chaves + 1) * TAM_ARQ_FOLHA + (max_chaves + 1) * sizeof(long);

    long pos = sizeof(TConfig);
    fseek(f, pos, SEEK_SET);

    TARVBMSDADOS infos;
    while (fread(&infos, sizeof(TARVBMSDADOS), 1, f) == 1)
    {
        if (infos.val == 0)
            return pos;
        pos += tam_no;
        fseek(f, pos, SEEK_SET);
    }
    return -1;
}

void TARVBMS_escreve_no(FILE *index, long pos, TARVBMSDADOS dados, char ids[][TAM_NOME], long *filhos, char nome_arq[][TAM_ARQ_FOLHA])
{
    TConfig c;
    rewind(index);

    fread(&c, sizeof(TConfig), 1, index);

    int max_chaves = (c.t * 2) - 1;
    fseek(index, pos, SEEK_SET);

    fwrite(&dados, sizeof(TARVBMSDADOS), 1, index);
    fwrite(ids, TAM_NOME, max_chaves, index);

    fwrite(nome_arq, TAM_ARQ_FOLHA, max_chaves + 1, index);

    fwrite(filhos, sizeof(long), max_chaves + 1, index);
}

long TARVBMS_cria_novo_no(FILE *index, TARVBMSDADOS dados, char ids[][TAM_NOME], long *filhos, char nome_arq[][TAM_ARQ_FOLHA])
{
    rewind(index);

    long pos = TARVBMS_busca_index_invalido(index);

    if (pos == -1)
    {
        fseek(index, 0, SEEK_END);
        pos = ftell(index);
    }

    TARVBMS_escreve_no(index, pos, dados, ids, filhos, nome_arq);

    return pos;
}

long TARVBMS_raiz(FILE *arq)
{
    long raiz;
    TConfig temp;
    fseek(arq, 0, SEEK_SET);
    fread(&temp, sizeof(TConfig), 1, arq);
    if (temp.raiz <= 0)
    {
        printf("Raiz nao encontrada!! Funcao TARVBMS_raiz\n");
        return -1;
    }
    raiz = temp.raiz;
    return raiz;
}

void TARVBMS_divide_no_interno(FILE *index, long pos, long pos_pai, int t)
{
    rewind(index);
    int max_chaves = (2 * t) - 1;
    int meio = t - 1;

    TARVBMSDADOS infos_ant;
    char ids_ant[max_chaves][TAM_NOME];
    long filhos_ant[max_chaves + 1];
    char arq_folha_ant[max_chaves + 1][TAM_ARQ_FOLHA];

    le_no_interno(index, pos, &infos_ant, ids_ant, filhos_ant, arq_folha_ant);

    TARVBMSDADOS infos_pai;
    char ids_pai[max_chaves][TAM_NOME];
    long filhos_pai[max_chaves + 1];
    char arq_folha_pai[max_chaves + 1][TAM_ARQ_FOLHA];

    le_no_interno(index, pos_pai, &infos_pai, ids_pai, filhos_pai, arq_folha_pai);

    TARVBMSDADOS infos_novo;
    char ids_novo[max_chaves][TAM_NOME];
    long filhos_novo[max_chaves + 1];
    char arq_folha_novo[max_chaves + 1][TAM_ARQ_FOLHA];

    for (int j = 0; j < max_chaves; j++)
        ids_novo[j][0] = '\0';
    for (int j = 0; j <= max_chaves; j++)
    {
        filhos_novo[j] = -1;
        arq_folha_novo[j][0] = '\0';
    }

    infos_novo.filho_folha = infos_ant.filho_folha;
    infos_novo.nchaves = t - 1;
    infos_novo.val = 1;

    for (int j = 0; j < t - 1; j++)
    {
        strcpy(ids_novo[j], ids_ant[j + t]);
        ids_ant[j + t][0] = '\0';
    }

    if (!infos_ant.filho_folha)
    {
        for (int j = 0; j < t; j++)
        {
            filhos_novo[j] = filhos_ant[j + t];
            filhos_ant[j + t] = -1;
        }
    }
    else
    {
        for (int j = 0; j < t; j++)
        {
            strcpy(arq_folha_novo[j], arq_folha_ant[j + t]);
            arq_folha_ant[j + t][0] = '\0';
        }
    }

    infos_ant.nchaves = t - 1;

    // Salva chave do meio antes de apagar
    char chave_meio[TAM_NOME];
    strcpy(chave_meio, ids_ant[meio]);
    ids_ant[meio][0] = '\0';

    int i = infos_pai.nchaves;
    while (i > 0 && filhos_pai[i] != pos)
        i--;
    i++;

    for (int j = infos_pai.nchaves; j >= i; j--)
        filhos_pai[j + 1] = filhos_pai[j];

    for (int j = infos_pai.nchaves - 1; j >= i - 1; j--)
        strcpy(ids_pai[j + 1], ids_pai[j]);

    strcpy(ids_pai[i - 1], chave_meio);
    infos_pai.nchaves++;

    long pos_novo = TARVBMS_cria_novo_no(index, infos_novo, ids_novo, filhos_novo, arq_folha_novo);
    filhos_pai[i] = pos_novo;

    TARVBMS_escreve_no(index, pos, infos_ant, ids_ant, filhos_ant, arq_folha_ant);
    TARVBMS_escreve_no(index, pos_pai, infos_pai, ids_pai, filhos_pai, arq_folha_pai);
}

void TARVBMS_divisao_folha(FILE *f, long pos_no, int i_filho, int indice_folha)
{
    TConfig config;
    rewind(f);
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;

    int tam_max = (2 * t) - 1;
    int indice_nova = config.indice;

    TFOLHA *folha_velha = TFOLHA_le(indice_folha, t);
    if (!folha_velha)
    {
        printf("Erro ao ler folha_%d!\n", indice_folha);
        exit(1);
    }

    TARVBMSDADOS infos_pai;
    char ids_pai[tam_max][TAM_NOME];
    long filhos_pai[tam_max + 1];
    char arq_folha_pai[tam_max + 1][TAM_ARQ_FOLHA];
    le_no_interno(f, pos_no, &infos_pai, ids_pai, filhos_pai, arq_folha_pai);

    if (pos_no != -1)
    {
        if (i_filho + 1 <= infos_pai.nchaves)
        {
            int k;
            sscanf(arq_folha_pai[i_filho + 1], "../dados/folha_%d.bin", &k);
            TFOLHA *dir = TFOLHA_le(k, t);
            if (dir && dir->dados.nchaves < t - 1)
            {
                TFOLHA *folha_temp = TFOLHA_aloca(t);
                for (int j = 0; j < t; j++)
                {
                    folha_temp->entidades[j] = folha_velha->entidades[j + t];
                }
                for (int j = t; j < tam_max; j++)
                {
                    folha_temp->entidades[j] = dir->entidades[j - t];
                }
                folha_temp->dados.nchaves = t + dir->dados.nchaves;
                folha_velha->dados.nchaves = t - 1;
                TFOLHA_cria(folha_temp, t, k);
                TFOLHA_cria(folha_velha, t, indice_folha);
            }
            else
            {
                printf("Erro não foi encontrado dir em divisão folha\n");
            }
            return;
        }
        if (i_filho - 1 >= 0)
        {
            int k;
            sscanf(arq_folha_pai[i_filho - 1], "../dados/folha_%d.bin", &k);
            TFOLHA *esq = TFOLHA_le(k, t);
            if (esq && esq->dados.nchaves < t - 1)
            {
                TFOLHA *folha_temp = TFOLHA_aloca(t);
                for (int j = 0; j < t; j++)
                {
                    folha_temp->entidades[j] = esq->entidades[j];
                }
                for (int j = t; j < tam_max; j++)
                {
                    folha_temp->entidades[j] = folha_velha->entidades[j - t];
                    folha_velha->entidades[j - t] = folha_velha->entidades[j];
                }
                folha_velha->dados.nchaves = t - 1;
                folha_temp->dados.nchaves = t + esq->dados.nchaves;
                TFOLHA_cria(folha_temp, t, k);
                TFOLHA_cria(folha_velha, t, indice_folha);
            }
            else
            {
                printf("Erro não foi encontrado esq em divisão folha\n");
            }
            return;
        }
    }
    TFOLHA *folha_nova = TFOLHA_aloca(t);
    config.indice++;

    for (int i = 0; i < t; i++)
        folha_nova->entidades[i] = folha_velha->entidades[(t - 1) + i];

    folha_nova->dados.nchaves = t;
    folha_velha->dados.nchaves = t - 1;

    strcpy(folha_nova->dados.prox, folha_velha->dados.prox);
    sprintf(folha_velha->dados.prox, "folha_%d.bin", indice_nova);

    TFOLHA_cria(folha_velha, t, indice_folha);

    TFOLHA_cria(folha_nova, t, indice_nova);

    char chave_promovida[TAM_NOME];
    if (folha_nova->entidades[0].tipo == TIPO_PESSOA)
        strcpy(chave_promovida, folha_nova->entidades[0].dados.pessoa.nome);
    else
        strcpy(chave_promovida, folha_nova->entidades[0].dados.filme.titulo);

    free(folha_velha->entidades);
    free(folha_velha);
    free(folha_nova->entidades);
    free(folha_nova);

    rewind(f);
    fwrite(&config, sizeof(TConfig), 1, f);

    TARVBMSDADOS infos_pai;
    char ids_pai[tam_max][TAM_NOME];
    long filhos_pai[tam_max + 1];
    char arq_folha_pai[tam_max + 1][TAM_ARQ_FOLHA];

    le_no_interno(f, pos_no, &infos_pai, ids_pai, filhos_pai, arq_folha_pai);

    for (int j = infos_pai.nchaves - 1; j >= i_filho; j--)
        strcpy(ids_pai[j + 1], ids_pai[j]);
    for (int j = infos_pai.nchaves; j > i_filho; j--)
        strcpy(arq_folha_pai[j + 1], arq_folha_pai[j]);

    strcpy(ids_pai[i_filho], chave_promovida);
    sprintf(arq_folha_pai[i_filho + 1], "folha_%d.bin", indice_nova);
    infos_pai.nchaves++;

    TARVBMS_escreve_no(f, pos_no, infos_pai, ids_pai, filhos_pai, arq_folha_pai);
    if (pos_no == -1)
    {
        TFOLHA *folha_nova = TFOLHA_aloca(t);
        config.indice++;

        for (int i = 0; i < t; i++)
            folha_nova->entidades[i] = folha_velha->entidades[(t - 1) + i];

        folha_nova->dados.nchaves = t;
        folha_velha->dados.nchaves = t - 1;

        strcpy(folha_nova->dados.prox, folha_velha->dados.prox);
        sprintf(folha_velha->dados.prox, "folha_%d.bin", indice_nova);

        TFOLHA_cria(folha_velha, t, indice_folha);

        TFOLHA_cria(folha_nova, t, indice_nova);

        char chave_promovida[TAM_NOME];
        if (folha_nova->entidades[0].tipo == TIPO_PESSOA)
            strcpy(chave_promovida, folha_nova->entidades[0].dados.pessoa.nome);
        else
            strcpy(chave_promovida, folha_nova->entidades[0].dados.filme.titulo);

        free(folha_velha->entidades);
        free(folha_velha);
        free(folha_nova->entidades);
        free(folha_nova);

        rewind(f);
        fwrite(&config, sizeof(TConfig), 1, f);

        TARVBMSDADOS nova_raiz = {1, 1, 1};
        char ids[tam_max][TAM_NOME];
        char arq_folha[tam_max + 1][TAM_ARQ_FOLHA];
        long filhos[tam_max + 1];

        for (int i = 0; i < tam_max; i++)
        {
            ids[i][0] = '\0';
            arq_folha[i][0] = '\0';
            filhos[i] = -1;
        }
        arq_folha[tam_max][0] = '\0';
        filhos[tam_max] = -1;

        strcpy(ids[0], chave_promovida);
        sprintf(arq_folha[0], "folha_%d.bin", indice_folha);
        sprintf(arq_folha[1], "folha_%d.bin", indice_nova);

        long pos_raiz = sizeof(TConfig);
        TARVBMS_escreve_no(f, pos_raiz, nova_raiz, ids, filhos, arq_folha);

        config.raiz = pos_raiz;
        rewind(f);
        fwrite(&config, sizeof(TConfig), 1, f);
        return;
    }
}

void TARVBMS_insere_nao_completo(FILE *f, long pos_no, TD *entidade, int t)
{
    int max_chaves = (2 * t) - 1;
    TARVBMSDADOS infos;
    char ids[max_chaves][TAM_NOME];
    long filhos[max_chaves + 1];
    char arq_folha[max_chaves + 1][TAM_ARQ_FOLHA];

    le_no_interno(f, pos_no, &infos, ids, filhos, arq_folha); // Le o no interno e poe no info

    char chave[TAM_NOME]; // Pega a chave da entidade a gnt qr inserir
    if (entidade->tipo == TIPO_PESSOA)
    {
        strcpy(chave, entidade->dados.pessoa.nome);
    }
    else
    {
        strcpy(chave, entidade->dados.filme.titulo);
    }

    int i = infos.nchaves - 1;
    while (i >= 0 && strcmp(chave, ids[i]) < 0)
        i--; // Logica rossetiana para achar o indice correto onde ele deve ser inserido
    i++;     // i fica com a pos exata do filho

    if (infos.filho_folha == 1) // Caso A: Os filho e folha
    {
        int indice_folha;
        sscanf(arq_folha[i], "folha_%d.bin", &indice_folha); // Extrai o indice da folha

        char arq_filho[30]; // Abrindo a folha p ver se ta cheia
        sprintf(arq_filho, "../dados/folha_%d.bin", indice_folha);
        FILE *ffolha = fopen(arq_filho, "rb");
        if (!ffolha)
        {
            printf("Erro ao abrir %s. Funcao insere nao completo, if_filho folha == 1 \n", arq_filho);
            exit(1);
        }
        TDADOSFOLHA dados_folha;
        fread(&dados_folha, sizeof(TDADOSFOLHA), 1, ffolha);
        fclose(ffolha);

        if (dados_folha.nchaves >= max_chaves)
        { // Folha esta cheia, precisa do split
            printf("A folha %d esta cheia. Comecando divisao\n", indice_folha);
            TARVBMS_divisao_folha(f, pos_no, i, indice_folha);
            //  Como a folha foi dividida, uma nova chave fantasia subiu para o nó atual
            //  O nó atual mudou Precisamos reler os seus dados para atualizar o nosso vetor de chaves em memória.
            le_no_interno(f, pos_no, &infos, ids, filhos, arq_folha);

            if (strcmp(chave, ids[i]) > 0) // Verificacao p ver se a chave que queremos inserir e maior que a chave que acabou de subir
                printf("FAAH\n");
            {
                i++;
                // Atualizamos o indice_folha para o número da nova folha
                sscanf(arq_folha[i], "folha_%d.bin", &indice_folha);
            }
        }

        printf("Inserindo chave '%s' na folha %d!\n", chave, indice_folha);
        TFOLHA_insere(entidade, t, indice_folha);
    }
    else
    {
        // CASO B: OS FILHOS SÃO NÓS INTERNOS
        long pos_filho = filhos[i];

        TARVBMSDADOS infos_filho;
        char ids_filho[max_chaves][100];
        long filhos_filho[max_chaves + 1];
        char arq_folha_filho[max_chaves + 1][30];

        le_no_interno(f, pos_filho, &infos_filho, ids_filho, filhos_filho, arq_folha_filho);

        if (infos_filho.nchaves == max_chaves)
        {
            TARVBMS_divide_no_interno(f, pos_filho, pos_no, t);
            le_no_interno(f, pos_no, &infos, ids, filhos, arq_folha); // recarrega pai

            if (strcmp(chave, ids[i]) > 0)
                i++;
            pos_filho = filhos[i];
        }

        TARVBMS_insere_nao_completo(f, pos_filho, entidade, t);
    }
}

void TARVBMS_insere_um(TD *entidade)
{
    if (!entidade)
    {
        printf("Nao ha entidade! Funcao insere_um TARVBMS");
        return;
    }
    FILE *f = fopen("../dados/index.bin", "rb+");
    if (!f)
    {
        printf("Erro na abertura do arquivo, TARBBMS_insere_um!");
        exit(1);
    }

    if (entidade->tipo == TIPO_FILME)
        printf("Inserindo %s na arvore\n", entidade->dados.filme.titulo);
    else
        printf("Inserindo %s na arvore\n", entidade->dados.pessoa.nome);

    TConfig config;
    fseek(f, 0, SEEK_SET);
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;
    int max_chaves = (2 * t) - 1;

    if (config.indice == 0)
    { // Avore esta vazia
        // Cria a primeira folha em memória principal e insere a entidade
        TFOLHA *nova_folha = TFOLHA_aloca(t);
        nova_folha->entidades[0] = *entidade;
        nova_folha->dados.nchaves = 1;
        strcpy(nova_folha->dados.prox, "-1");
        // printf("%s FAAAAH\n", nova_folha->entidades[0].dados.pessoa.nome);

        // Grava a folha no seu arquivo binário distinto (folha_0.bin)
        TFOLHA_cria(nova_folha, t, 0);
        config.indice = 1; // Incrementa, pois agora temos a folha_0

        fseek(f, 0, SEEK_SET);
        fwrite(&config, sizeof(TConfig), 1, f);

        free(nova_folha->entidades);
        free(nova_folha);
        fclose(f);
        return;
    }
    if (config.raiz == -1)
    { // Caso em que a da p inserir na folha 0

        char arq[TAM_ARQ_FOLHA];
        sprintf(arq, "../dados/folha_0.bin");
        FILE *ffolha = fopen(arq, "rb");

        if (!ffolha)
        {
            printf("Erro: folha_0.bin não encontrada!\n");
            fclose(f);
            return;
        }

        TDADOSFOLHA dados_folha;
        fread(&dados_folha, sizeof(TDADOSFOLHA), 1, ffolha);
        fclose(ffolha);

        if (dados_folha.nchaves < max_chaves)
        {
            // Tem espaço, só inserir direto na folha 0
            TFOLHA_insere(entidade, t, 0);
        }
        else
        {
            TARVBMS_divisao_folha(f, -1, 0, 0);
            fclose(f);
            TARVBMS_insere_um(entidade);
            return;
        }
        fclose(f);
        return;
    }
    // raiz existe
    TARVBMSDADOS info_raiz;
    char ids_raiz[max_chaves][TAM_NOME];
    long filhos_raiz[max_chaves + 1];
    char arq_folha_raiz[max_chaves + 1][TAM_ARQ_FOLHA];

    le_no_interno(f, config.raiz, &info_raiz, ids_raiz, filhos_raiz, arq_folha_raiz);

    if (info_raiz.nchaves == max_chaves)
    {
        if (info_raiz.filho_folha)
        {
            int indice_folha_cheia;
            sscanf(arq_folha_raiz[info_raiz.nchaves], "folha_%d.bin", &indice_folha_cheia);
            printf("A raiz esta cheia. Iniciar logica de SPLIT de raiz!\n");
            TARVBMS_divisao_folha(f, config.raiz, info_raiz.nchaves, indice_folha_cheia);
        }
        else
        {
            TARVBMSDADOS infos_novo_pai;
            infos_novo_pai.nchaves = 0;
            infos_novo_pai.val = 1;
            infos_novo_pai.filho_folha = 0;

            char ids_novo_pai[max_chaves][TAM_NOME];
            long filhos_novo_pai[max_chaves + 1];
            char arq_folha_novo_pai[max_chaves + 1][TAM_ARQ_FOLHA];

            for (int i = 0; i < max_chaves; i++)
                ids_novo_pai[i][0] = '\0';
            for (int i = 0; i <= max_chaves; i++)
            {
                filhos_novo_pai[i] = -1;
                arq_folha_novo_pai[i][0] = '\0';
            }

            filhos_novo_pai[0] = config.raiz;

            long pos_novo_pai = TARVBMS_cria_novo_no(f, infos_novo_pai, ids_novo_pai, filhos_novo_pai, arq_folha_novo_pai);
            TARVBMS_divide_no_interno(f, config.raiz, pos_novo_pai, t);

            config.raiz = pos_novo_pai;
            fseek(f, 0, SEEK_SET);
            fwrite(&config, sizeof(TConfig), 1, f);
        }

        fclose(f);
        TARVBMS_insere_um(entidade); // reinsere com raiz dividida
        return;
    }

    else
    {
        printf("Descendo na arvore\n");
        TARVBMS_insere_nao_completo(f, config.raiz, entidade, t);
        fclose(f);
    }
}

void TARVBMS_removezada(FILE *f, long pos_no, char *chave, int t)
{
    int max_chaves = (2 * t) - 1;
    int min_chaves = t - 1;
    TARVBMSDADOS info;
    char vet_chaves[max_chaves][TAM_NOME];
    long filhos[max_chaves + 1];
    char arq_folha[max_chaves + 1][TAM_ARQ_FOLHA];

    // Le o no interno atual
    le_no_interno(f, pos_no, &info, vet_chaves, filhos, arq_folha);

    int i = 0;
    while (i < info.nchaves && strcmp(chave, vet_chaves[i]) >= 0)
        i++;

    if (info.filho_folha == 1)
    {
        // Fihlo e folha
        int indice_folha_alvo;
        sscanf(arq_folha[i], "folha_%d.bin", &indice_folha_alvo);
        int sucesso = TARVBMS_remove_da_folha(indice_folha_alvo, chave, t);
        // Ve se consegue remover da folha
        if (sucesso)
        {
            TFOLHA *folha_alvo = TFOLHA_le(indice_folha_alvo, t);
            // O registo foi apagado! Agora precisamos verificar se a folha ta belezinha!
            if (folha_alvo->dados.nchaves < min_chaves) // Folha ta ruim, precisamos pegar emprestado
            {
                printf("A folha %d vai precisar pegar emprestado!\n", indice_folha_alvo);
                int emprestou = 0;
                if (i > 0 && !emprestou)
                {

                    int indice_esq;
                    sscanf(arq_folha[i - 1], "folha_%d.bin", &indice_esq);
                    TFOLHA *folha_esq = TFOLHA_le(indice_esq, t);
                    if (folha_esq->dados.nchaves > min_chaves)
                    {
                        /*printf("Emprestando da folha %d (Dir) para a %d (Alvo)\n", indice_dir, indice_folha_alvo);

                        folha_alvo->entidades[folha_alvo->dados.nchaves] = folha_dir->entidades[0];*/
                        // Aonde o gusmao 15 parou
                    }
                }

                // FALTA FAZER AQ!!!!! Tentar Empréstimo do irmão da esquerda (i - 1) ou direita (i + 1)

                // Se não der para emprestar, fazer Fusão com irmão da esquerda ou direita
            }
            free(folha_alvo->entidades);
            free(folha_alvo);
        }
    }
    else
    {
        // FIlho e no interno
        long pos_filho = filhos[i];
        // TARVBMS_retira_recursivo(f, pos_filho, chave, t);
        //  Fazer a logica dos nores interno
    }
}

int TARVBMS_remove_da_folha(int indice_folha, char *chave, int t)
{
    TFOLHA *folha = TFOLHA_le(indice_folha, t);
    if (!folha)
    {
        printf("Erro ao ler folha_%d para remocao.\n", indice_folha);
        return 0;
    }
    int i = 0;
    int encontra = 0;
    for (i = 0; i < folha->dados.nchaves; i++)
    {
        char chave_atual[100]; // Acha a chave q vai remover
        if (folha->entidades[i].tipo == TIPO_PESSOA)
        {
            strcpy(chave_atual, folha->entidades[i].dados.pessoa.nome);
        }
        else
        {
            strcpy(chave_atual, folha->entidades[i].dados.filme.titulo);
        }

        if (strcmp(chave_atual, chave) == 0)
        {
            encontra = 1;
            break;
        }
    }
    if (!encontra)
    { // Se n encontrou na folha
        printf("A chave '%s' nao foi encontrada na folha %d.\n", chave, indice_folha);
        free(folha->entidades);
        free(folha);
        return 0;
    }
    for (int j = 0; j < folha->dados.nchaves - 1; j++)
    {
        folha->entidades[j] = folha->entidades[j + 1]; // Reorganiza o vetor de entidades;
    }
    folha->dados.nchaves--;
    TFOLHA_cria(folha, t, indice_folha);
    free(folha->entidades);
    free(folha);
    printf("Chave '%s' removida com sucesso da folha %d!\n", chave, indice_folha);
    return 1;
}

void TARVBMS_retira(char *chave)
{
    FILE *f = fopen("../dados/index.bin", "rb+");
    if (!f)
    {
        printf("Erro na abertura do arquivo, TARBBMS_retira!");
        exit(1);
    }
    TConfig config;
    rewind(f);
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;
    if (config.indice == 0)
    {
        printf("Arvore vazia nao ha nada para retirar!\n");
        fclose(f);
        return;
    }
    if (config.raiz == -1 && config.indice > 0)
    { // Caso que so tem a folha.
        TARVBMS_remove_da_folha(0, chave, t);
        fclose(f);
        return;
    }
    TARVBMS_removezada(f, config.raiz, chave, t);

    fclose(f);
}

void TARVBMS_preenche_arvore(char *txt)
{
    FILE *fteste = fopen("../dados/index.bin", "rb");
    if (!fteste)
    {
        printf("Erro ao abrir arquivo de indice para preenche arvore\n");
        exit(1);
    }
    fseek(fteste, 0, SEEK_END);

    if (ftell(fteste) > sizeof(TConfig))
    {
        fclose(fteste);
        return;
    }
    fclose(fteste);

    printf("Preenchendo a arvore...\n");

    FILE *finput = fopen(txt, "r");
    if (!finput)
    {
        printf("Erro ao abrir arquivo de entrada: %s\n", txt);
        exit(1);
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), finput))
    {
        // Remove quebras de linha (\n e \r) de forma segura
        linha[strcspn(linha, "\r\n")] = '\0';

        // Ignora linhas vazias
        if (strlen(linha) == 0)
            continue;

        char *token = strtok(linha, "|"); // Tipo
        if (!token)
            continue;

        trim(token);
        TipoEntidade tipo;

        if (strcmp(token, "Movie") == 0)
        {
            tipo = TIPO_FILME;

            token = strtok(NULL, "|"); // TITULO
            if (!token)
                continue; // Proteção contra NULL
            trim(token);
            char titulo[100];
            strncpy(titulo, token, sizeof(titulo) - 1);
            titulo[sizeof(titulo) - 1] = '\0';

            token = strtok(NULL, "|"); // ANO
            if (!token)
                continue; // Proteção contra NULL
            trim(token);
            int ano = (int)strtol(token, NULL, 10);

            token = strtok(NULL, "|"); // SUBT
            if (!token)
                continue; // Proteção contra NULL
            trim(token);
            char sub[200];
            strncpy(sub, token, sizeof(sub) - 1);
            sub[sizeof(sub) - 1] = '\0';

            TD entidade;
            entidade.tipo = tipo;
            entidade.dados.filme.ano = ano;
            strcpy(entidade.dados.filme.titulo, titulo);
            strcpy(entidade.dados.filme.sub, sub);
            TARVBMS_insere_um(&entidade);
        }
        else if (strcmp(token, "Person") == 0) // Garante que só entra se for Person válido
        {
            tipo = TIPO_PESSOA;

            token = strtok(NULL, "|"); // NOME
            if (!token)
                continue; // Proteção contra NULL
            trim(token);
            char nome[100];
            strncpy(nome, token, sizeof(nome) - 1);
            nome[sizeof(nome) - 1] = '\0';

            token = strtok(NULL, "|"); // ANO
            if (!token)
                continue; // Proteção contra NULL
            trim(token);
            int ano = (int)strtol(token, NULL, 10);

            TD entidade;
            entidade.tipo = tipo;
            strcpy(entidade.dados.pessoa.nome, nome);
            entidade.dados.pessoa.ano = ano;
            TARVBMS_insere_um(&entidade);
        }
    }
    fclose(finput);
}
