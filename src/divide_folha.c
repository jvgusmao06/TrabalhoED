#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME 100
#define TAM_ARQ_FOLHA 30

typedef enum { TIPO_PESSOA, TIPO_FILME } TipoEntidade;

typedef struct {
    int nchaves;
    int filho_folha;
    int val;
} TARVBMSDADOS;

typedef struct {
    long raiz;
    int t;
    int indice;
} TConfig;

typedef struct {
    int nchaves;
    char prox[TAM_ARQ_FOLHA];
} TDADOSFOLHA;

typedef struct {
    struct { char nome[TAM_NOME]; int ano; } pessoa;
    struct { char titulo[TAM_NOME]; int ano; char sub[200]; } filme;
} TDados;

typedef struct {
    TipoEntidade tipo;
    TDados dados;
} TD;

typedef struct {
    TDADOSFOLHA dados;
    TD *entidades;
} TFOLHA;

// ---- Protótipos ----
void le_no_interno(FILE *f, long posicao, TARVBMSDADOS *infos,
                   char (*ids)[TAM_NOME], long *filhos, char (*arq_folha)[TAM_ARQ_FOLHA]);
void TARVBMS_escreve_no(FILE *index, long pos, TARVBMSDADOS dados,
                         char ids[][TAM_NOME], long *filhos, char nome_arq[][TAM_ARQ_FOLHA]);
long TARVBMS_cria_novo_no(FILE *index, TARVBMSDADOS dados, char ids[][TAM_NOME],
                           long *filhos, char nome_arq[][TAM_ARQ_FOLHA]);
long TARVBMS_busca_index_invalido(FILE *f);
void TARVBMS_divisao_folha(FILE *f, long pos_no, int i_filho, int indice_folha);
void TARVBMS_divide_raiz_folha(FILE *f);
TFOLHA *TFOLHA_aloca(int t);
TFOLHA *TFOLHA_le(int indice, int t);
void TFOLHA_cria(TFOLHA *folha, int t, int indice);
void imprime_no(FILE *f, long pos, const char *label);
void imprime_folha(int indice, int t);

// ---- Implementações mínimas ----

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

void TARVBMS_escreve_no(FILE *index, long pos, TARVBMSDADOS dados,
                         char ids[][TAM_NOME], long *filhos, char nome_arq[][TAM_ARQ_FOLHA])
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

long TARVBMS_busca_index_invalido(FILE *f)
{
    rewind(f);
    TConfig c;
    fread(&c, sizeof(TConfig), 1, f);
    int max_chaves = (c.t * 2) - 1;

    int tam_no = sizeof(TARVBMSDADOS)
               + max_chaves * TAM_NOME
               + (max_chaves + 1) * TAM_ARQ_FOLHA
               + (max_chaves + 1) * sizeof(long);

    long pos = sizeof(TConfig);
    fseek(f, pos, SEEK_SET);

    TARVBMSDADOS infos;
    while (fread(&infos, sizeof(TARVBMSDADOS), 1, f) == 1)
    {
        if (infos.val == 0) return pos;
        pos += tam_no;
        fseek(f, pos, SEEK_SET);
    }
    return -1;
}

long TARVBMS_cria_novo_no(FILE *index, TARVBMSDADOS dados, char ids[][TAM_NOME],
                           long *filhos, char nome_arq[][TAM_ARQ_FOLHA])
{
    long pos = TARVBMS_busca_index_invalido(index);
    if (pos == -1) { fseek(index, 0, SEEK_END); pos = ftell(index); }
    TARVBMS_escreve_no(index, pos, dados, ids, filhos, nome_arq);
    return pos;
}

TFOLHA *TFOLHA_aloca(int t)
{
    TFOLHA *f = malloc(sizeof(TFOLHA));
    f->entidades = malloc(sizeof(TD) * (2 * t - 1));
    f->dados.nchaves = 0;
    strcpy(f->dados.prox, "-1");
    return f;
}

void TFOLHA_cria(TFOLHA *folha, int t, int indice)
{
    char arq[50];
    sprintf(arq, "folha_%d.bin", indice);
    FILE *f = fopen(arq, "wb");
    fwrite(&folha->dados, sizeof(TDADOSFOLHA), 1, f);
    fwrite(folha->entidades, sizeof(TD), 2 * t - 1, f);
    fclose(f);
}

TFOLHA *TFOLHA_le(int indice, int t)
{
    char arq[50];
    sprintf(arq, "folha_%d.bin", indice);
    FILE *f = fopen(arq, "rb");
    if (!f) return NULL;
    TFOLHA *folha = TFOLHA_aloca(t);
    fread(&folha->dados, sizeof(TDADOSFOLHA), 1, f);
    fread(folha->entidades, sizeof(TD), 2 * t - 1, f);
    fclose(f);
    return folha;
}

void TARVBMS_divisao_folha(FILE *f, long pos_no, int i_filho, int indice_folha) // tem que mexer em tudo
{
    TConfig config;
    rewind(f);
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;
    int tam_max = (2 * t) - 1;
    int indice_nova = config.indice;

    TFOLHA *folha_velha = TFOLHA_le(indice_folha, t);
    if (!folha_velha) { printf("Erro ao ler folha_%d!\n", indice_folha); exit(1); }

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

    free(folha_velha->entidades); free(folha_velha);
    free(folha_nova->entidades);  free(folha_nova);

    rewind(f);
    fwrite(&config, sizeof(TConfig), 1, f);

    if (pos_no == -1)
    {
        TARVBMSDADOS nova_raiz = {1, 1, 1};
        char ids[tam_max][TAM_NOME];
        char arq_folha[tam_max + 1][TAM_ARQ_FOLHA];
        long filhos[tam_max + 1];

        for (int i = 0; i < tam_max; i++) { ids[i][0] = '\0'; arq_folha[i][0] = '\0'; filhos[i] = -1; }
        arq_folha[tam_max][0] = '\0'; filhos[tam_max] = -1;

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
}

void TARVBMS_divide_raiz_folha(FILE *f)
{
    if (!f) return;

    TConfig config;
    rewind(f);
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;
    int tam_max = (2 * t) - 1;

    TARVBMSDADOS infos_raiz;
    char ids_raiz[tam_max][TAM_NOME];
    long filhos_raiz[tam_max + 1];
    char arq_folha_raiz[tam_max + 1][TAM_ARQ_FOLHA];

    le_no_interno(f, config.raiz, &infos_raiz, ids_raiz, filhos_raiz, arq_folha_raiz);

    int indice_folha_cheia;
    sscanf(arq_folha_raiz[infos_raiz.nchaves], "folha_%d.bin", &indice_folha_cheia);

    TARVBMS_divisao_folha(f, config.raiz, infos_raiz.nchaves, indice_folha_cheia);
}

// ---- Utilitários de impressão ----

void imprime_no(FILE *f, long pos, const char *label)
{
    TConfig cfg; rewind(f); fread(&cfg, sizeof(TConfig), 1, f);
    int max_chaves = (2 * cfg.t) - 1;

    TARVBMSDADOS infos;
    char ids[max_chaves][TAM_NOME];
    long filhos[max_chaves + 1];
    char arq_folha[max_chaves + 1][TAM_ARQ_FOLHA];

    le_no_interno(f, pos, &infos, ids, filhos, arq_folha);

    printf("\n=== %s (pos=%ld) ===\n", label, pos);
    printf("  nchaves=%d  filho_folha=%d\n", infos.nchaves, infos.filho_folha);
    printf("  chaves: ");
    for (int i = 0; i < infos.nchaves; i++) printf("[%s] ", ids[i]);
    printf("\n");
    if (infos.filho_folha)
    {
        printf("  arq_folha: ");
        for (int i = 0; i <= infos.nchaves; i++) printf("[%s] ", arq_folha[i]);
        printf("\n");
    }
    else
    {
        printf("  filhos: ");
        for (int i = 0; i <= infos.nchaves; i++) printf("[%ld] ", filhos[i]);
        printf("\n");
    }
}

void imprime_folha(int indice, int t)
{
    TFOLHA *f = TFOLHA_le(indice, t);
    if (!f) { printf("  folha_%d: nao encontrada\n", indice); return; }
    printf("  folha_%d: nchaves=%d prox=%s | ", indice, f->dados.nchaves, f->dados.prox);
    for (int i = 0; i < f->dados.nchaves; i++)
    {
        if (f->entidades[i].tipo == TIPO_PESSOA)
            printf("[%s] ", f->entidades[i].dados.pessoa.nome);
        else
            printf("[%s] ", f->entidades[i].dados.filme.titulo);
    }
    printf("\n");
    free(f->entidades); free(f);
}

// ---- Helpers para montar cenários ----

FILE *inicializa(int t)
{
    FILE *f = fopen("teste_divisao.bin", "wb+");
    TConfig cfg = {-1, t, 0};
    fwrite(&cfg, sizeof(TConfig), 1, f);
    return f;
}

// Cria folha com pessoas de nomes dados, grava no disco e atualiza config.indice
int cria_folha_pessoas(FILE *f, char (*nomes)[TAM_NOME], int n, char *prox)
{
    TConfig cfg; rewind(f); fread(&cfg, sizeof(TConfig), 1, f);
    int t = cfg.t;
    int indice = cfg.indice;

    TFOLHA *folha = TFOLHA_aloca(t);
    folha->dados.nchaves = n;
    strcpy(folha->dados.prox, prox);
    for (int i = 0; i < n; i++)
    {
        folha->entidades[i].tipo = TIPO_PESSOA;
        strcpy(folha->entidades[i].dados.pessoa.nome, nomes[i]);
        folha->entidades[i].dados.pessoa.ano = 1990 + i;
    }
    TFOLHA_cria(folha, t, indice);
    free(folha->entidades); free(folha);

    cfg.indice++;
    rewind(f); fwrite(&cfg, sizeof(TConfig), 1, f);
    return indice;
}

// Cria nó interno folha-filho com arq_folha dado
long cria_no_raiz_folha(FILE *f, char (*chaves)[TAM_NOME], char (*arqs)[TAM_ARQ_FOLHA], int nchaves)
{
    TConfig cfg; rewind(f); fread(&cfg, sizeof(TConfig), 1, f);
    int t = cfg.t;
    int max_chaves = (2 * t) - 1;

    TARVBMSDADOS d = {nchaves, 1, 1};
    char ids[max_chaves][TAM_NOME];
    long filhos[max_chaves + 1];
    char arq_folha[max_chaves + 1][TAM_ARQ_FOLHA];

    for (int i = 0; i < max_chaves; i++) { ids[i][0] = '\0'; arq_folha[i][0] = '\0'; filhos[i] = -1; }
    arq_folha[max_chaves][0] = '\0'; filhos[max_chaves] = -1;

    for (int i = 0; i < nchaves; i++) strcpy(ids[i], chaves[i]);
    for (int i = 0; i <= nchaves; i++) strcpy(arq_folha[i], arqs[i]);

    fseek(f, 0, SEEK_END);
    long pos = ftell(f);
    TARVBMS_escreve_no(f, pos, d, ids, filhos, arq_folha);

    cfg.raiz = pos;
    rewind(f); fwrite(&cfg, sizeof(TConfig), 1, f);
    return pos;
}

void limpa_folhas(int n)
{
    for (int i = 0; i < n; i++)
    {
        char arq[50];
        sprintf(arq, "folha_%d.bin", i);
        remove(arq);
    }
}

// ---- Casos de teste ----

// Caso 1: arvore so tem folha_0 cheia, sem raiz interna (pos_no = -1)
void teste_caso1()
{
    printf("\n========================================\n");
    printf("CASO 1: folha unica cheia, sem raiz (pos_no=-1)\n");
    printf("========================================\n");

    FILE *f = inicializa(2); // t=2, max=3

    char nomes[3][TAM_NOME] = {"Ana", "Bia", "Carlos"};
    cria_folha_pessoas(f, nomes, 3, "-1");

    printf("Antes:\n");
    imprime_folha(0, 2);

    TARVBMS_divisao_folha(f, -1, 0, 0);

    TConfig cfg; rewind(f); fread(&cfg, sizeof(TConfig), 1, f);
    printf("\nDepois (raiz em pos=%ld):\n", cfg.raiz);
    imprime_no(f, cfg.raiz, "Raiz nova");
    imprime_folha(0, 2);
    imprime_folha(1, 2);

    fclose(f);
    remove("teste_divisao.bin");
    limpa_folhas(2);
}

// Caso 2: raiz cheia com filho_folha=1 (divide_raiz_folha)
void teste_caso2()
{
    printf("\n========================================\n");
    printf("CASO 2: raiz cheia com filho_folha=1 (divide_raiz_folha)\n");
    printf("========================================\n");

    FILE *f = inicializa(2);

    // Cria 4 folhas encadeadas
    char n0[1][TAM_NOME] = {"Ana"};         int i0 = cria_folha_pessoas(f, n0, 1, "folha_1.bin");
    char n1[1][TAM_NOME] = {"Bia"};         int i1 = cria_folha_pessoas(f, n1, 1, "folha_2.bin");
    char n2[1][TAM_NOME] = {"Carlos"};      int i2 = cria_folha_pessoas(f, n2, 1, "folha_3.bin");
    char n3[1][TAM_NOME] = {"Daniela"};     int i3 = cria_folha_pessoas(f, n3, 1, "-1");

    // Raiz com 3 chaves apontando para as 4 folhas
    char chaves[3][TAM_NOME] = {"Bia", "Carlos", "Daniela"};
    char arqs[4][TAM_ARQ_FOLHA] = {"folha_0.bin","folha_1.bin","folha_2.bin","folha_3.bin"};
    long pos_raiz = cria_no_raiz_folha(f, chaves, arqs, 3);

    printf("Antes:\n");
    imprime_no(f, pos_raiz, "Raiz (cheia)");
    for (int i = 0; i < 4; i++) imprime_folha(i, 2);

    TARVBMS_divide_raiz_folha(f);

    TConfig cfg; rewind(f); fread(&cfg, sizeof(TConfig), 1, f);
    printf("\nDepois (raiz em pos=%ld):\n", cfg.raiz);
    imprime_no(f, cfg.raiz, "Raiz nova");

    TARVBMSDADOS info; char ids[3][TAM_NOME]; long filhos[4]; char arq_folha[4][TAM_ARQ_FOLHA];
    le_no_interno(f, cfg.raiz, &info, ids, filhos, arq_folha);
    imprime_no(f, filhos[0], "Filho esquerdo");
    imprime_no(f, filhos[1], "Filho direito");

    fclose(f);
    remove("teste_divisao.bin");
    limpa_folhas(6);
}

// Caso 3: divisao_folha com pai existente, folha no meio
void teste_caso3()
{
    printf("\n========================================\n");
    printf("CASO 3: divisao_folha com pai existente, folha no meio\n");
    printf("========================================\n");

    FILE *f = inicializa(2);

    // 3 folhas: folha_0 [Ana], folha_1 [Bia,Carlos,Daniela] (cheia), folha_2 [Eduardo]
    char n0[1][TAM_NOME] = {"Ana"};
    char n1[3][TAM_NOME] = {"Bia","Carlos","Daniela"};
    char n2[1][TAM_NOME] = {"Eduardo"};
    cria_folha_pessoas(f, n0, 1, "folha_1.bin");
    cria_folha_pessoas(f, n1, 3, "folha_2.bin");
    cria_folha_pessoas(f, n2, 1, "-1");

    // Raiz com 2 chaves: [Bia, Eduardo], filhos: [folha_0, folha_1, folha_2]
    char chaves[3][TAM_NOME] = {"Bia","Eduardo",""};
    char arqs[4][TAM_ARQ_FOLHA] = {"folha_0.bin","folha_1.bin","folha_2.bin",""};
    long pos_raiz = cria_no_raiz_folha(f, chaves, arqs, 2);

    printf("Antes:\n");
    imprime_no(f, pos_raiz, "Raiz");
    for (int i = 0; i < 3; i++) imprime_folha(i, 2);

    // Divide folha_1 (indice 1, que e o i_filho=1 da raiz)
    TARVBMS_divisao_folha(f, pos_raiz, 1, 1);

    TConfig cfg; rewind(f); fread(&cfg, sizeof(TConfig), 1, f);
    printf("\nDepois:\n");
    imprime_no(f, cfg.raiz, "Raiz");
    for (int i = 0; i < 4; i++) imprime_folha(i, 2);

    fclose(f);
    remove("teste_divisao.bin");
    limpa_folhas(4);
}

int main()
{
    teste_caso1();
    teste_caso2();
    teste_caso3();
    printf("\nTodos os testes concluidos.\n");
    return 0;
}