#include <stdlib.h>
#include <string.h>
#include "../header/TARVBMS.h"
#include "../header/TFOLHA.h"

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
/* ======================================================
   Função auxiliar: extrai a chave (nome/título) de uma entidade
   ====================================================== */

void get_chave_entidade(TD *e, char *chave)
{
    if (e->tipo == TIPO_PESSOA)
        strcpy(chave, e->dados.pessoa.nome);
    else
        strcpy(chave, e->dados.filme.titulo);
}

/* ======================================================
   Inicialização
   ====================================================== */

void TARVBMS_inicializa(int t)
{
    FILE *f = fopen("../dados/index.bin", "wb");
    if (!f)
    {
        printf("Falha ao criar arquivo index.bin\n");
        exit(1);
    }
    TConfig temp;
    temp.t = t;
    temp.raiz = -1;
    temp.indice = 0;
    temp.preenchido = 0;
    fwrite(&temp, sizeof(TConfig), 1, f);
    fclose(f);
}

/* ======================================================
   Leitura/Escrita de nós internos
   ====================================================== */

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

void TARVBMS_escreve_no(FILE *index, long pos, TARVBMSDADOS dados,
                        char ids[][TAM_NOME], long *filhos,
                        char nome_arq[][TAM_ARQ_FOLHA])
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

long TARVBMS_cria_novo_no(FILE *index, TARVBMSDADOS dados,
                          char ids[][TAM_NOME], long *filhos,
                          char nome_arq[][TAM_ARQ_FOLHA])
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
    TConfig temp;
    fseek(arq, 0, SEEK_SET);
    fread(&temp, sizeof(TConfig), 1, arq);
    if (temp.raiz == -1)
    {
        printf("Raiz nao encontrada!\n");
        return -1;
    }
    return temp.raiz;
}

/* ======================================================
   Divisão de nó interno
   ====================================================== */

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

    /* Prepara o novo nó */
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

    /* Copia a metade direita das chaves para o novo nó */
    for (int j = 0; j < t - 1; j++)
    {
        strcpy(ids_novo[j], ids_ant[j + t]);
        ids_ant[j + t][0] = '\0';
    }

    /* Copia os filhos/folhas correspondentes */
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

    /* Salva chave do meio (será promovida) */
    char chave_meio[TAM_NOME];
    strcpy(chave_meio, ids_ant[meio]);
    ids_ant[meio][0] = '\0';

    /* Encontra a posição do nó dividido no pai */
    int i = infos_pai.nchaves;
    while (i > 0 && filhos_pai[i] != pos)
        i--;
    i++;

    /* Desloca filhos e chaves do pai para abrir espaço */
    for (int j = infos_pai.nchaves; j >= i; j--)
        filhos_pai[j + 1] = filhos_pai[j];
    for (int j = infos_pai.nchaves - 1; j >= i - 1; j--)
        strcpy(ids_pai[j + 1], ids_pai[j]);

    strcpy(ids_pai[i - 1], chave_meio);
    infos_pai.nchaves++;

    long pos_novo = TARVBMS_cria_novo_no(index, infos_novo, ids_novo,
                                         filhos_novo, arq_folha_novo);
    filhos_pai[i] = pos_novo;

    TARVBMS_escreve_no(index, pos, infos_ant, ids_ant, filhos_ant, arq_folha_ant);
    TARVBMS_escreve_no(index, pos_pai, infos_pai, ids_pai, filhos_pai, arq_folha_pai);
}

/* ======================================================
   Divisão de folha
   ====================================================== */

void TARVBMS_divisao_folha(FILE *f, long pos_no, int i_filho, int indice_folha)
{
    TConfig config;
    rewind(f);
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;
    int tam_max = (2 * t) - 1;
    int indice_nova = config.indice;

    /* 1. Lê a folha cheia */
    TFOLHA *folha_velha = TFOLHA_le(indice_folha, t);
    if (!folha_velha)
    {
        printf("Erro ao ler folha_%d!\n", indice_folha);
        exit(1);
    }

    /* 2. Cria nova folha e distribui entradas */
    TFOLHA *folha_nova = TFOLHA_aloca(t);
    config.indice++;

    for (int i = 0; i < t; i++)
        folha_nova->entidades[i] = folha_velha->entidades[(t - 1) + i];

    folha_nova->dados.nchaves = t;
    folha_velha->dados.nchaves = t - 1;

    /* 3. Atualiza encadeamento */
    strcpy(folha_nova->dados.prox, folha_velha->dados.prox);
    sprintf(folha_velha->dados.prox, "folha_%d.bin", indice_nova);

    /* 4. Grava ambas as folhas em disco */
    TFOLHA_cria(folha_velha, t, indice_folha);
    TFOLHA_cria(folha_nova, t, indice_nova);

    /* 5. Determina chave promovida (primeira chave da nova folha) */
    char chave_promovida[TAM_NOME];
    get_chave_entidade(&folha_nova->entidades[0], chave_promovida);

    /* 6. Libera memória */
    free(folha_velha->entidades);
    free(folha_velha);
    free(folha_nova->entidades);
    free(folha_nova);

    /* 7. Atualiza config */
    rewind(f);
    fwrite(&config, sizeof(TConfig), 1, f);

    /* 8. Promove chave para o pai */
    if (pos_no == -1)
    {
        /* Sem pai → cria nova raiz */
        TARVBMSDADOS nova_raiz;
        nova_raiz.filho_folha = 1;
        nova_raiz.val = 1;
        nova_raiz.nchaves = 1;

        char ids[tam_max][TAM_NOME];
        char arq_folha[tam_max + 1][TAM_ARQ_FOLHA];
        long filhos[tam_max + 1];

        for (int i = 0; i < tam_max; i++)
        {
            ids[i][0] = '\0';
            filhos[i] = -1;
            arq_folha[i][0] = '\0';
        }
        filhos[tam_max] = -1;
        arq_folha[tam_max][0] = '\0';

        strcpy(ids[0], chave_promovida);
        sprintf(arq_folha[0], "folha_%d.bin", indice_folha);
        sprintf(arq_folha[1], "folha_%d.bin", indice_nova);

        long pos_raiz = TARVBMS_cria_novo_no(f, nova_raiz, ids, filhos, arq_folha);

        config.raiz = pos_raiz;
        rewind(f);
        fwrite(&config, sizeof(TConfig), 1, f);
    }
    else
    {
        /* Pai existe → insere chave promovida no pai */
        TARVBMSDADOS infos_pai;
        char ids_pai[tam_max][TAM_NOME];
        long filhos_pai[tam_max + 1];
        char arq_folha_pai[tam_max + 1][TAM_ARQ_FOLHA];

        le_no_interno(f, pos_no, &infos_pai, ids_pai, filhos_pai, arq_folha_pai);

        /* Desloca chaves e arq_folha para abrir espaço na posição i_filho */
        for (int j = infos_pai.nchaves - 1; j >= i_filho; j--)
            strcpy(ids_pai[j + 1], ids_pai[j]);
        for (int j = infos_pai.nchaves; j > i_filho; j--)
            strcpy(arq_folha_pai[j + 1], arq_folha_pai[j]);

        strcpy(ids_pai[i_filho], chave_promovida);
        sprintf(arq_folha_pai[i_filho + 1], "folha_%d.bin", indice_nova);
        infos_pai.nchaves++;

        TARVBMS_escreve_no(f, pos_no, infos_pai, ids_pai, filhos_pai, arq_folha_pai);
    }
}

/* ======================================================
   Inserção em nó não-completo (descida recursiva)
   ====================================================== */

void TARVBMS_insere_nao_completo(FILE *f, long pos_no, TD *entidade, int t)
{
    int max_chaves = (2 * t) - 1;
    TARVBMSDADOS infos;
    char ids[max_chaves][TAM_NOME];
    long filhos[max_chaves + 1];
    char arq_folha[max_chaves + 1][TAM_ARQ_FOLHA];

    le_no_interno(f, pos_no, &infos, ids, filhos, arq_folha);

    char chave[TAM_NOME];
    get_chave_entidade(entidade, chave);

    /* Encontra o índice do filho correto */
    int i = infos.nchaves - 1;
    while (i >= 0 && strcmp(chave, ids[i]) < 0)
        i--;
    i++;

    if (infos.filho_folha == 1)
    {
        /* Caso A: filhos são folhas */
        int indice_folha;
        sscanf(arq_folha[i], "folha_%d.bin", &indice_folha);

        /* Verifica se a folha está cheia */
        char arq_filho[50];
        sprintf(arq_filho, "../dados/folha_%d.bin", indice_folha);
        FILE *ffolha = fopen(arq_filho, "rb");
        if (!ffolha)
        {
            printf("Erro ao abrir %s\n", arq_filho);
            exit(1);
        }
        TDADOSFOLHA dados_folha;
        fread(&dados_folha, sizeof(TDADOSFOLHA), 1, ffolha);
        fclose(ffolha);

        if (dados_folha.nchaves >= max_chaves)
        {
            /* Folha cheia → divide */
            TARVBMS_divisao_folha(f, pos_no, i, indice_folha);

            /* Relê o nó pai (agora tem uma nova chave na posição i) */
            le_no_interno(f, pos_no, &infos, ids, filhos, arq_folha);

            /* Decide para qual folha ir */
            if (strcmp(chave, ids[i]) >= 0)
                i++;
            sscanf(arq_folha[i], "folha_%d.bin", &indice_folha);
        }

        TFOLHA_insere(entidade, t, indice_folha);
    }
    else
    {
        /* Caso B: filhos são nós internos */
        long pos_filho = filhos[i];

        TARVBMSDADOS infos_filho;
        char ids_filho[max_chaves][TAM_NOME];
        long filhos_filho[max_chaves + 1];
        char arq_folha_filho[max_chaves + 1][TAM_ARQ_FOLHA];

        le_no_interno(f, pos_filho, &infos_filho, ids_filho, filhos_filho, arq_folha_filho);

        if (infos_filho.nchaves == max_chaves)
        {
            /* Filho cheio → divide o nó interno */
            TARVBMS_divide_no_interno(f, pos_filho, pos_no, t);

            /* Relê o nó pai */
            le_no_interno(f, pos_no, &infos, ids, filhos, arq_folha);

            if (strcmp(chave, ids[i]) > 0)
                i++;
            pos_filho = filhos[i];
        }

        TARVBMS_insere_nao_completo(f, pos_filho, entidade, t);
    }
}

/* ======================================================
   Inserção de uma entidade
   ====================================================== */

void TARVBMS_insere(TD *entidade)
{
    if (!entidade)
    {
        printf("Entidade nula!\n");
        return;
    }

    FILE *f = fopen("../dados/index.bin", "rb+");
    if (!f)
    {
        printf("Erro ao abrir index.bin!\n");
        exit(1);
    }

    TConfig config;
    fseek(f, 0, SEEK_SET);
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;
    int max_chaves = (2 * t) - 1;

    if (config.indice == 0)
    {
        /* Árvore vazia → cria primeira folha */
        TFOLHA *nova_folha = TFOLHA_aloca(t);
        nova_folha->entidades[0] = *entidade;
        nova_folha->dados.nchaves = 1;
        strcpy(nova_folha->dados.prox, "-1");

        TFOLHA_cria(nova_folha, t, 0);
        config.indice = 1;

        fseek(f, 0, SEEK_SET);
        fwrite(&config, sizeof(TConfig), 1, f);

        free(nova_folha->entidades);
        free(nova_folha);
        fclose(f);
        return;
    }

    if (config.raiz == -1)
    {
        /* Sem raiz → inserir diretamente na folha_0 */
        FILE *ffolha = fopen("../dados/folha_0.bin", "rb");
        if (!ffolha)
        {
            printf("Erro: folha_0.bin nao encontrada!\n");
            fclose(f);
            return;
        }
        TDADOSFOLHA dados_folha;
        fread(&dados_folha, sizeof(TDADOSFOLHA), 1, ffolha);
        fclose(ffolha);

        if (dados_folha.nchaves < max_chaves)
        {
            TFOLHA_insere(entidade, t, 0);
        }
        else
        {
            /* folha_0 cheia → split, criando a raiz */
            TARVBMS_divisao_folha(f, -1, 0, 0);
            fclose(f);
            TARVBMS_insere(entidade);
            return;
        }
        fclose(f);
        return;
    }

    /* Raiz existe */
    TARVBMSDADOS info_raiz;
    char ids_raiz[max_chaves][TAM_NOME];
    long filhos_raiz[max_chaves + 1];
    char arq_folha_raiz[max_chaves + 1][TAM_ARQ_FOLHA];
    le_no_interno(f, config.raiz, &info_raiz, ids_raiz, filhos_raiz, arq_folha_raiz);

    if (info_raiz.nchaves == max_chaves)
    {
        /* Raiz cheia → cria novo pai vazio e divide a raiz */
        TARVBMSDADOS infos_novo_pai;
        infos_novo_pai.nchaves = 0;
        infos_novo_pai.val = 1;
        infos_novo_pai.filho_folha = 0; /* filhos são nós internos */

        char ids_np[max_chaves][TAM_NOME];
        long filhos_np[max_chaves + 1];
        char arq_folha_np[max_chaves + 1][TAM_ARQ_FOLHA];

        for (int i = 0; i < max_chaves; i++)
            ids_np[i][0] = '\0';
        for (int i = 0; i <= max_chaves; i++)
        {
            filhos_np[i] = -1;
            arq_folha_np[i][0] = '\0';
        }

        filhos_np[0] = config.raiz;

        long pos_novo_pai = TARVBMS_cria_novo_no(f, infos_novo_pai, ids_np,
                                                 filhos_np, arq_folha_np);
        TARVBMS_divide_no_interno(f, config.raiz, pos_novo_pai, t);

        config.raiz = pos_novo_pai;
        fseek(f, 0, SEEK_SET);
        fwrite(&config, sizeof(TConfig), 1, f);

        fclose(f);
        TARVBMS_insere(entidade);
        return;
    }

    TARVBMS_insere_nao_completo(f, config.raiz, entidade, t);
    fclose(f);
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
    for (int j = i; j < folha->dados.nchaves - 1; j++)
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
                        printf("Emprestando da folha %d (Esq) para a folha %d (Alvo)\n", indice_esq, indice_folha_alvo);

                        // Como vem da esquerda, temos de abrir espaço no início da folha alvo
                        for (int j = folha_alvo->dados.nchaves; j > 0; j--)
                        {
                            folha_alvo->entidades[j] = folha_alvo->entidades[j - 1];
                        }

                        // Pega a última entidade da folha da esquerda e coloca no índice 0 do alvo
                        folha_alvo->entidades[0] = folha_esq->entidades[folha_esq->dados.nchaves - 1];
                        folha_alvo->dados.nchaves++;
                        folha_esq->dados.nchaves--;

                        // Atualiza a chave separadora no nó interno pai
                        char nova_chave[TAM_NOME];
                        if (folha_alvo->entidades[0].tipo == TIPO_PESSOA)
                            strcpy(nova_chave, folha_alvo->entidades[0].dados.pessoa.nome);
                        else
                            strcpy(nova_chave, folha_alvo->entidades[0].dados.filme.titulo);

                        strcpy(vet_chaves[i - 1], nova_chave);

                        // Guarda no disco
                        TFOLHA_cria(folha_esq, t, indice_esq);
                        TFOLHA_cria(folha_alvo, t, indice_folha_alvo);
                        TARVBMS_escreve_no(f, pos_no, info, vet_chaves, filhos, arq_folha);

                        emprestou = 1;
                    }
                    if (folha_esq)
                    {
                        free(folha_esq->entidades);
                        free(folha_esq);
                    }
                }

                // FALTA FAZER AQ!!!!! Tentar Empréstimo do irmão da esquerda (i - 1) ou direita (i + 1)
                // FIZ!
                if (i < info.nchaves && !emprestou)
                {
                    int indice_dir;
                    sscanf(arq_folha[i + 1], "folha_%d.bin", &indice_dir);
                    TFOLHA *folha_dir = TFOLHA_le(indice_dir, t);

                    if (folha_dir && folha_dir->dados.nchaves > min_chaves)
                    {
                        printf("Emprestando da folha %d (Dir) para a folha %d (Alvo)\n", indice_dir, indice_folha_alvo);

                        // Pega na primeira entidade da folha da direita e coloca no fim do alvo
                        folha_alvo->entidades[folha_alvo->dados.nchaves] = folha_dir->entidades[0];
                        folha_alvo->dados.nchaves++;

                        // Reorganiza a folha da direita (puxa tudo para o início para tapar o buraco)
                        for (int j = 0; j < folha_dir->dados.nchaves - 1; j++)
                        {
                            folha_dir->entidades[j] = folha_dir->entidades[j + 1];
                        }
                        folha_dir->dados.nchaves--;

                        // Atualiza a chave separadora no nó interno pai
                        char nova_chave[TAM_NOME];
                        if (folha_dir->entidades[0].tipo == TIPO_PESSOA)
                            strcpy(nova_chave, folha_dir->entidades[0].dados.pessoa.nome);
                        else
                            strcpy(nova_chave, folha_dir->entidades[0].dados.filme.titulo);

                        strcpy(vet_chaves[i], nova_chave);

                        // Guarda no disco
                        TFOLHA_cria(folha_dir, t, indice_dir);
                        TFOLHA_cria(folha_alvo, t, indice_folha_alvo);
                        TARVBMS_escreve_no(f, pos_no, info, vet_chaves, filhos, arq_folha);

                        emprestou = 1;
                    }
                    if (folha_dir)
                    {
                        free(folha_dir->entidades);
                        free(folha_dir);
                    }
                }

                // Se não der para emprestar, fazer Fusão com irmão da esquerda ou direita
                if (!emprestou)
                {
                    int pos_fusao = (i > 0) ? (i - 1) : i;
                    int indice_esq, indice_dir;

                    sscanf(arq_folha[pos_fusao], "folha_%d.bin", &indice_esq);
                    sscanf(arq_folha[pos_fusao + 1], "folha_%d.bin", &indice_dir);

                    printf("Fundindo folhas %d (Esq) e %d (Dir)\n", indice_esq, indice_dir);

                    TFOLHA *folha_esq = TFOLHA_le(indice_esq, t);
                    TFOLHA *folha_dir = TFOLHA_le(indice_dir, t);

                    // Juntar as entidades: Move as chaves da folha direita para o final da folha esquerda
                    for (int j = 0; j < folha_dir->dados.nchaves; j++)
                    {
                        folha_esq->entidades[folha_esq->dados.nchaves + j] = folha_dir->entidades[j];
                    }
                    folha_esq->dados.nchaves += folha_dir->dados.nchaves;

                    // Remendar o ponteiro da lista encadeada das folhas
                    strcpy(folha_esq->dados.prox, folha_dir->dados.prox);

                    // Atualizar o Pai: Apaga a chave e o ponteiro para a folha que foi fundida
                    for (int j = pos_fusao; j < info.nchaves - 1; j++)
                    {
                        strcpy(vet_chaves[j], vet_chaves[j + 1]);
                    }
                    for (int j = pos_fusao + 1; j < info.nchaves; j++)
                    {
                        strcpy(arq_folha[j], arq_folha[j + 1]);
                        filhos[j] = filhos[j + 1]; // CORREÇÃO AQUI
                    }
                    info.nchaves--;

                    // Guarda a folha fundida e o nó interno atualizado
                    TFOLHA_cria(folha_esq, t, indice_esq);
                    TARVBMS_escreve_no(f, pos_no, info, vet_chaves, filhos, arq_folha);

                    free(folha_esq->entidades);
                    free(folha_esq);
                    free(folha_dir->entidades);
                    free(folha_dir);
                }
            } // Fecho correto do IF do underflow

            free(folha_alvo->entidades);
            free(folha_alvo);
        } // Fecho correto do if(sucesso)
    } // Fecho do if (info.filho_folha == 1)
    else
    {
        // FIlho e no interno
        // TARVBMS_retira_recursivo(f, pos_filho, chave, t);

        // Chamada Recursiva: Desce na árvore para remover o elemento
        long pos_filho = filhos[i];
        TARVBMS_removezada(f, pos_filho, chave, t);

        // Ao voltar da recursão, lemos o filho para ver se ele sofreu underflow
        TARVBMSDADOS info_filho;
        char vet_chaves_filho[max_chaves][TAM_NOME];
        long filhos_filho[max_chaves + 1];
        char arq_folha_filho[max_chaves + 1][TAM_ARQ_FOLHA];

        le_no_interno(f, pos_filho, &info_filho, vet_chaves_filho, filhos_filho, arq_folha_filho);

        if (info_filho.nchaves < min_chaves && pos_filho != TARVBMS_raiz(f))
        {
            printf("No interno em %ld em underflow! Rebalanceando...\n", pos_filho);
            int emprestou = 0;

            // --- TENTATIVA 1: Emprestar do irmão da ESQUERDA ---
            if (i > 0 && !emprestou)
            {
                long pos_esq = filhos[i - 1];
                TARVBMSDADOS info_esq;
                char chaves_esq[max_chaves][TAM_NOME];
                long filhos_esq[max_chaves + 1];
                char folhas_esq[max_chaves + 1][TAM_ARQ_FOLHA];

                le_no_interno(f, pos_esq, &info_esq, chaves_esq, filhos_esq, folhas_esq);

                if (info_esq.nchaves > min_chaves)
                {
                    // Abre espaço no filho com underflow
                    for (int j = info_filho.nchaves; j > 0; j--)
                    {
                        strcpy(vet_chaves_filho[j], vet_chaves_filho[j - 1]);
                        filhos_filho[j + 1] = filhos_filho[j];
                        strcpy(arq_folha_filho[j + 1], arq_folha_filho[j]);
                    }
                    filhos_filho[1] = filhos_filho[0];
                    strcpy(arq_folha_filho[1], arq_folha_filho[0]);

                    // A chave do pai desce para o filho
                    strcpy(vet_chaves_filho[0], vet_chaves[i - 1]);
                    // O último ponteiro da esquerda vai para o início do filho
                    filhos_filho[0] = filhos_esq[info_esq.nchaves];
                    strcpy(arq_folha_filho[0], folhas_esq[info_esq.nchaves]);

                    info_filho.nchaves++;

                    // A última chave da esquerda sobe para o pai
                    strcpy(vet_chaves[i - 1], chaves_esq[info_esq.nchaves - 1]);
                    info_esq.nchaves--;

                    // Guarda tudo
                    TARVBMS_escreve_no(f, pos_esq, info_esq, chaves_esq, filhos_esq, folhas_esq);
                    TARVBMS_escreve_no(f, pos_filho, info_filho, vet_chaves_filho, filhos_filho, arq_folha_filho);
                    TARVBMS_escreve_no(f, pos_no, info, vet_chaves, filhos, arq_folha);
                    emprestou = 1;
                }
            }

            // --- TENTATIVA 2: Emprestar do irmão da DIREITA ---
            if (i < info.nchaves && !emprestou)
            {
                long pos_dir = filhos[i + 1];
                TARVBMSDADOS info_dir;
                char chaves_dir[max_chaves][TAM_NOME];
                long filhos_dir[max_chaves + 1];
                char folhas_dir[max_chaves + 1][TAM_ARQ_FOLHA];

                le_no_interno(f, pos_dir, &info_dir, chaves_dir, filhos_dir, folhas_dir);

                if (info_dir.nchaves > min_chaves)
                {
                    // A chave do pai desce para o final do filho com underflow
                    strcpy(vet_chaves_filho[info_filho.nchaves], vet_chaves[i]);
                    // O primeiro ponteiro da direita vai para o final do filho
                    filhos_filho[info_filho.nchaves + 1] = filhos_dir[0];
                    strcpy(arq_folha_filho[info_filho.nchaves + 1], folhas_dir[0]);

                    info_filho.nchaves++;

                    // A primeira chave da direita sobe para o pai
                    strcpy(vet_chaves[i], chaves_dir[0]);

                    // Puxa os elementos da direita para o início (tapando o buraco)
                    for (int j = 0; j < info_dir.nchaves - 1; j++)
                    {
                        strcpy(chaves_dir[j], chaves_dir[j + 1]);
                        filhos_dir[j] = filhos_dir[j + 1];
                        strcpy(folhas_dir[j], folhas_dir[j + 1]);
                    }
                    filhos_dir[info_dir.nchaves - 1] = filhos_dir[info_dir.nchaves];
                    strcpy(folhas_dir[info_dir.nchaves - 1], folhas_dir[info_dir.nchaves]);

                    info_dir.nchaves--;

                    // Guarda tudo
                    TARVBMS_escreve_no(f, pos_dir, info_dir, chaves_dir, filhos_dir, folhas_dir);
                    TARVBMS_escreve_no(f, pos_filho, info_filho, vet_chaves_filho, filhos_filho, arq_folha_filho);
                    TARVBMS_escreve_no(f, pos_no, info, vet_chaves, filhos, arq_folha);
                    emprestou = 1;
                }
            }

            // --- TENTATIVA 3: FUSÃO (Merge) de Nós Internos ---
            if (!emprestou)
            {
                int pos_fusao = (i > 0) ? (i - 1) : i;
                long pos_esq = filhos[pos_fusao];
                long pos_dir = filhos[pos_fusao + 1];

                TARVBMSDADOS info_esq, info_dir;
                char chaves_esq[max_chaves][TAM_NOME], chaves_dir[max_chaves][TAM_NOME];
                long filhos_esq[max_chaves + 1], filhos_dir[max_chaves + 1];
                char folhas_esq[max_chaves + 1][TAM_ARQ_FOLHA], folhas_dir[max_chaves + 1][TAM_ARQ_FOLHA];

                le_no_interno(f, pos_esq, &info_esq, chaves_esq, filhos_esq, folhas_esq);
                le_no_interno(f, pos_dir, &info_dir, chaves_dir, filhos_dir, folhas_dir);

                // Descer a chave do pai para o nó da esquerda
                strcpy(chaves_esq[info_esq.nchaves], vet_chaves[pos_fusao]);
                info_esq.nchaves++;

                // Copiar tudo do nó da direita para o nó da esquerda
                for (int j = 0; j < info_dir.nchaves; j++)
                {
                    strcpy(chaves_esq[info_esq.nchaves + j], chaves_dir[j]);
                    filhos_esq[info_esq.nchaves + j] = filhos_dir[j];
                    strcpy(folhas_esq[info_esq.nchaves + j], folhas_dir[j]);
                }
                filhos_esq[info_esq.nchaves + info_dir.nchaves] = filhos_dir[info_dir.nchaves];
                strcpy(folhas_esq[info_esq.nchaves + info_dir.nchaves], folhas_dir[info_dir.nchaves]);

                info_esq.nchaves += info_dir.nchaves;

                // Atualizar o Pai: Apagar a chave que desceu e o ponteiro da direita
                for (int j = pos_fusao; j < info.nchaves - 1; j++)
                {
                    strcpy(vet_chaves[j], vet_chaves[j + 1]);
                }
                for (int j = pos_fusao + 1; j < info.nchaves; j++)
                {
                    filhos[j] = filhos[j + 1];
                }
                info.nchaves--;

                // Info_dir "morre" logicamente. Podemos marcar como inválido se tiver um campo para isso.
                info_dir.val = 0;

                // Guarda as alterações
                TARVBMS_escreve_no(f, pos_esq, info_esq, chaves_esq, filhos_esq, folhas_esq);
                TARVBMS_escreve_no(f, pos_dir, info_dir, chaves_dir, filhos_dir, folhas_dir);
                TARVBMS_escreve_no(f, pos_no, info, vet_chaves, filhos, arq_folha);
            }
        }
    }
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

    int max_chaves = (2 * t) - 1;
    TARVBMSDADOS info_raiz;
    char ids_raiz[max_chaves][TAM_NOME];
    long filhos_raiz[max_chaves + 1];
    char arq_folha_raiz[max_chaves + 1][TAM_ARQ_FOLHA];

    le_no_interno(f, config.raiz, &info_raiz, ids_raiz, filhos_raiz, arq_folha_raiz);
    // Se a raiz fico vazia
    if (info_raiz.nchaves == 0)
    {
        if (info_raiz.filho_folha == 0)
        {
            // A raiz apontava para nós internos. O filho resultante da fusão vira a nova raiz!
            config.raiz = filhos_raiz[0];
            printf(">>> A raiz fundiu e a arvore encolheu de altura! Nova raiz em: %ld <<<\n", config.raiz);
        }
        else
        {
            // A raiz apontava diretamente para folhas, e agora só sobrou uma folha na árvore.
            // A árvore volta ao seu estado inicial (sem nós internos)!
            config.raiz = -1;
            printf(">>> A arvore encolheu tanto que agora so possui a folha base! <<<\n");
        }

        fseek(f, 0, SEEK_SET);
        fwrite(&config, sizeof(TConfig), 1, f);
    }
    fclose(f);
}

void imprime_folha(FILE *f, char *nome_arq, int nivel, int t)
{
    int indice;
    if (sscanf(nome_arq, "folha_%d.bin", &indice) != 1)
        return;

    TFOLHA *folha = TFOLHA_le(indice, t);
    if (!folha)
        return;

    for (int k = folha->dados.nchaves - 1; k >= 0; k--)
    {
        char chave[TAM_NOME];
        get_chave_entidade(&folha->entidades[k], chave);
        for (int j = 0; j < nivel; j++)
            printf("\t\t");
        printf("{%s}\n", chave);
    }

    free(folha->entidades);
    free(folha);
}

void imprime_no_recursivo(FILE *f, long pos, int nivel, int t)
{
    int max_chaves = (2 * t) - 1;
    TARVBMSDADOS info;
    char ids[max_chaves][TAM_NOME];
    long filhos[max_chaves + 1];
    char arq_folha[max_chaves + 1][TAM_ARQ_FOLHA];

    le_no_interno(f, pos, &info, ids, filhos, arq_folha);

    if (info.filho_folha == 0)
    {
        imprime_no_recursivo(f, filhos[info.nchaves], nivel + 1, t);

        for (int i = info.nchaves - 1; i >= 0; i--)
        {
            for (int j = 0; j < nivel; j++)
                printf("\t\t");
            printf("{%s}\n", ids[i]);

            imprime_no_recursivo(f, filhos[i], nivel + 1, t);
        }
    }
    else
    {
        imprime_folha(f, arq_folha[info.nchaves], nivel + 1, t);

        for (int i = info.nchaves - 1; i >= 0; i--)
        {
            for (int j = 0; j < nivel; j++)
                printf("\t\t");
            printf("{%s}\n", ids[i]);

            imprime_folha(f, arq_folha[i], nivel + 1, t);
        }
    }
}

void TARVBMS_imprime()
{
    FILE *f = fopen("../dados/index.bin", "rb");
    if (!f)
    {
        printf("Erro ao abrir index.bin\n");
        return;
    }

    TConfig config;
    fread(&config, sizeof(TConfig), 1, f);

    if (config.indice == 0)
    {
        printf("Arvore vazia!\n");
        fclose(f);
        return;
    }

    if (config.raiz == -1)
    {
        printf("Apenas folha_0:\n");
        TFOLHA *folha = TFOLHA_le(0, config.t);
        if (folha)
        {
            TFOLHA_imprime(folha);
            free(folha->entidades);
            free(folha);
        }
        fclose(f);
        return;
    }

    printf("=== Arvore B+ (t=%d) ===\n", config.t);
    imprime_no_recursivo(f, config.raiz, 0, config.t);

    fclose(f);
}

/* ======================================================
   Impressão — todas as folhas (percorre encadeamento)
   ====================================================== */

void TARVBMS_imprime_folhas()
{
    FILE *f = fopen("../dados/index.bin", "rb");
    if (!f)
    {
        printf("Erro ao abrir index.bin\n");
        return;
    }

    TConfig config;
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;

    if (config.indice == 0)
    {
        printf("Arvore vazia!\n");
        fclose(f);
        return;
    }

    /* Encontra a folha mais à esquerda */
    int indice_folha;

    if (config.raiz == -1)
    {
        indice_folha = 0;
    }
    else
    {
        int max_chaves = (2 * t) - 1;
        long pos = config.raiz;
        TARVBMSDADOS info;
        char ids[max_chaves][TAM_NOME];
        long filhos[max_chaves + 1];
        char arq_folha_arr[max_chaves + 1][TAM_ARQ_FOLHA];

        while (1)
        {
            le_no_interno(f, pos, &info, ids, filhos, arq_folha_arr);
            if (info.filho_folha)
            {
                sscanf(arq_folha_arr[0], "folha_%d.bin", &indice_folha);
                break;
            }
            pos = filhos[0];
        }
    }

    fclose(f);

    /* Percorre todas as folhas pelo encadeamento */
    printf("=== Folhas da Arvore B+ ===\n");
    int total = 0;
    while (1)
    {
        TFOLHA *folha = TFOLHA_le(indice_folha, t);
        if (!folha)
            break;

        printf("--- folha_%d.bin (nchaves=%d, prox=%s) ---\n",
               indice_folha, folha->dados.nchaves, folha->dados.prox);
        for (int j = 0; j < folha->dados.nchaves; j++)
        {
            char chave[TAM_NOME];
            get_chave_entidade(&folha->entidades[j], chave);
            if (folha->entidades[j].tipo == TIPO_PESSOA)
                printf("  [Pessoa] %s (ano: %d)\n", chave, folha->entidades[j].dados.pessoa.ano);
            else
                printf("  [Filme]  %s (ano: %d)\n", chave, folha->entidades[j].dados.filme.ano);
        }
        total += folha->dados.nchaves;

        char prox[TAM_ARQ_FOLHA];
        strcpy(prox, folha->dados.prox);
        free(folha->entidades);
        free(folha);

        if (strcmp(prox, "-1") == 0)
            break;
        sscanf(prox, "folha_%d.bin", &indice_folha);
    }
    printf("\nTotal de entradas nas folhas: %d\n", total);
}

/* ======================================================
   Preenchimento da árvore a partir de arquivo texto
   ====================================================== */

void TARVBMS_preenche_arvore(char *txt)
{
    /* Se já existem dados além do TConfig, não preenche novamente */
    FILE *fteste = fopen("../dados/index.bin", "rb");
    if (!fteste)
    {
        printf("Erro ao abrir index.bin\n");
        exit(1);
    }

    TConfig c;
    fread(&c, sizeof(TConfig), 1, fteste);
    fclose(fteste);
    if (c.preenchido)
    {
        return;
    }

    printf("Preenchendo a arvore...\n");

    FILE *finput = fopen(txt, "r");
    if (!finput)
    {
        printf("Erro ao abrir arquivo de entrada: %s\n", txt);
        exit(1);
    }

    char linha[512];
    int count = 0;
    while (fgets(linha, sizeof(linha), finput))
    {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (strlen(linha) == 0)
            continue;

        char copia[512];
        strcpy(copia, linha);

        char *token = strtok(copia, "|");
        if (!token)
            continue;
        trim(token);

        if (strcmp(token, "Movie") == 0)
        {
            token = strtok(NULL, "|");
            if (!token)
                continue;
            trim(token);
            char titulo[TAM_NOME];
            strncpy(titulo, token, sizeof(titulo) - 1);
            titulo[sizeof(titulo) - 1] = '\0';

            token = strtok(NULL, "|");
            if (!token)
                continue;
            trim(token);
            int ano = (int)strtol(token, NULL, 10);

            token = strtok(NULL, "|");
            char sub[200];
            if (token)
            {
                trim(token);
                strncpy(sub, token, sizeof(sub) - 1);
                sub[sizeof(sub) - 1] = '\0';
            }
            else
            {
                sub[0] = '\0';
            }

            TD entidade;
            entidade.tipo = TIPO_FILME;
            entidade.dados.filme.ano = ano;
            strcpy(entidade.dados.filme.titulo, titulo);
            strcpy(entidade.dados.filme.sub, sub);
            TARVBMS_insere(&entidade);
            count++;
        }
        else if (strcmp(token, "Person") == 0)
        {
            token = strtok(NULL, "|");
            if (!token)
                continue;
            trim(token);
            char nome[TAM_NOME];
            strncpy(nome, token, sizeof(nome) - 1);
            nome[sizeof(nome) - 1] = '\0';

            token = strtok(NULL, "|");
            if (!token)
                continue;
            trim(token);
            int ano = (int)strtol(token, NULL, 10);

            TD entidade;
            entidade.tipo = TIPO_PESSOA;
            strcpy(entidade.dados.pessoa.nome, nome);
            entidade.dados.pessoa.ano = ano;
            TARVBMS_insere(&entidade);
            count++;
        }
    }
    fclose(finput);
    printf("Total de %d entidades inseridas.\n", count);
    FILE *fconfig = fopen("../dados/index.bin", "rb+");

    if (!fconfig)
    {
        printf("Erro ao abrir index.bin para marcar como preenchido\n");
        exit(1);
    }
    TConfig atual;
    fread(&atual, sizeof(TConfig), 1, fconfig);
    atual.preenchido = 1;
    rewind(fconfig);
    fwrite(&atual, sizeof(TConfig), 1, fconfig);
    fclose(fconfig);
}