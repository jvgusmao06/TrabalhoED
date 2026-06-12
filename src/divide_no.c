#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constantes de teste
#define TAM_NOME 100
#define TAM_ARQ_FOLHA 30

// t = 2, entao max_chaves = 3
int T = 2;

// Structs minimas para o teste
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

// ---- Protótipos ----
void le_no_interno(FILE *f, long posicao, TARVBMSDADOS *infos, char (*ids)[TAM_NOME], long *filhos, char (*arq_folha)[TAM_ARQ_FOLHA]);
void TARVBMS_escreve_no(FILE *index, long pos, TARVBMSDADOS dados, char ids[][TAM_NOME], long *filhos, char nome_arq[][TAM_ARQ_FOLHA]);
long TARVBMS_cria_novo_no(FILE *index, TARVBMSDADOS dados, char ids[][TAM_NOME], long *filhos, char nome_arq[][TAM_ARQ_FOLHA]);
long TARVBMS_busca_index_invalido(FILE *f);
void TARVBMS_divide_no_interno(FILE *index, long pos, long pos_pai, int t);
void imprime_no(FILE *f, long pos, const char *label);

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

    // Tamanho fixo de cada no: header + ids + arq_folha + filhos
    int tam_no = sizeof(TARVBMSDADOS)
               + max_chaves * TAM_NOME
               + (max_chaves + 1) * TAM_ARQ_FOLHA
               + (max_chaves + 1) * sizeof(long);

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

long TARVBMS_cria_novo_no(FILE *index, TARVBMSDADOS dados, char ids[][TAM_NOME],
                           long *filhos, char nome_arq[][TAM_ARQ_FOLHA])
{
    long pos = TARVBMS_busca_index_invalido(index);
    if (pos == -1)
    {
        fseek(index, 0, SEEK_END);
        pos = ftell(index);
    }
    TARVBMS_escreve_no(index, pos, dados, ids, filhos, nome_arq);
    return pos;
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

    for (int j = 0; j < max_chaves; j++) ids_novo[j][0] = '\0';
    for (int j = 0; j <= max_chaves; j++) { filhos_novo[j] = -1; arq_folha_novo[j][0] = '\0'; }

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

// ---- Utilitário de impressão ----

void imprime_no(FILE *f, long pos, const char *label)
{
    TConfig cfg;
    rewind(f);
    fread(&cfg, sizeof(TConfig), 1, f);
    int max_chaves = (2 * cfg.t) - 1;

    TARVBMSDADOS infos;
    char ids[max_chaves][TAM_NOME];
    long filhos[max_chaves + 1];
    char arq_folha[max_chaves + 1][TAM_ARQ_FOLHA];

    le_no_interno(f, pos, &infos, ids, filhos, arq_folha);

    printf("\n=== %s (pos=%ld) ===\n", label, pos);
    printf("  nchaves=%d  filho_folha=%d  val=%d\n",
           infos.nchaves, infos.filho_folha, infos.val);
    printf("  chaves: ");
    for (int i = 0; i < infos.nchaves; i++)
        printf("[%s] ", ids[i]);
    printf("\n");

    if (infos.filho_folha)
    {
        printf("  arq_folha: ");
        for (int i = 0; i <= infos.nchaves; i++)
            printf("[%s] ", arq_folha[i]);
    }
    else
    {
        printf("  filhos: ");
        for (int i = 0; i <= infos.nchaves; i++)
            printf("[%ld] ", filhos[i]);
    }
    printf("\n");
}

// ---- Helpers para montar nós de teste ----

long cria_no_folha(FILE *f, char (*chaves)[TAM_NOME], char (*folhas)[TAM_ARQ_FOLHA], int nchaves)
{
    TConfig cfg;
    rewind(f);
    fread(&cfg, sizeof(TConfig), 1, f);
    int max_chaves = (2 * cfg.t) - 1;

    TARVBMSDADOS d = {nchaves, 1, 1};
    char ids[max_chaves][TAM_NOME];
    long filhos[max_chaves + 1];
    char arq[max_chaves + 1][TAM_ARQ_FOLHA];

    for (int i = 0; i < max_chaves; i++) { ids[i][0] = '\0'; arq[i][0] = '\0'; filhos[i] = -1; }
    arq[max_chaves][0] = '\0'; filhos[max_chaves] = -1;

    for (int i = 0; i < nchaves; i++) { strcpy(ids[i], chaves[i]); strcpy(arq[i], folhas[i]); }
    strcpy(arq[nchaves], folhas[nchaves]);

    return TARVBMS_cria_novo_no(f, d, ids, filhos, arq);
}

long cria_no_interno(FILE *f, char (*chaves)[TAM_NOME], long *filhos_in, int nchaves)
{
    TConfig cfg;
    rewind(f);
    fread(&cfg, sizeof(TConfig), 1, f);
    int max_chaves = (2 * cfg.t) - 1;

    TARVBMSDADOS d = {nchaves, 0, 1};
    char ids[max_chaves][TAM_NOME];
    long filhos[max_chaves + 1];
    char arq[max_chaves + 1][TAM_ARQ_FOLHA];

    for (int i = 0; i < max_chaves; i++) { ids[i][0] = '\0'; arq[i][0] = '\0'; filhos[i] = -1; }
    arq[max_chaves][0] = '\0'; filhos[max_chaves] = -1;

    for (int i = 0; i < nchaves; i++) strcpy(ids[i], chaves[i]);
    for (int i = 0; i <= nchaves; i++) filhos[i] = filhos_in[i];

    return TARVBMS_cria_novo_no(f, d, ids, filhos, arq);
}

FILE *inicializa_arquivo(int t)
{
    FILE *f = fopen("teste_divide.bin", "wb+");
    TConfig cfg = { -1, t, 0 };
    fwrite(&cfg, sizeof(TConfig), 1, f);
    return f;
}

// ---- Casos de teste ----

// Caso 1: no cheio com filhos folha, pai vazio
void teste_caso1()
{
    printf("\n========================================\n");
    printf("CASO 1: no cheio (filho_folha=1), pai vazio\n");
    printf("========================================\n");

    FILE *f = inicializa_arquivo(2); // t=2, max=3

    // No cheio: chaves [A, B, C], aponta para folha_0..folha_3
    char chaves[3][TAM_NOME] = {"A", "B", "C"};
    char folhas[4][TAM_ARQ_FOLHA] = {"folha_0.bin","folha_1.bin","folha_2.bin","folha_3.bin"};
    long pos_filho = cria_no_folha(f, chaves, folhas, 3);

    // Pai vazio com filho apontando para pos_filho
    char chaves_pai[3][TAM_NOME] = {"", "", ""};
    long filhos_pai[4] = {pos_filho, -1, -1, -1};
    long pos_pai = cria_no_interno(f, chaves_pai, filhos_pai, 0);

    printf("Antes:\n");
    imprime_no(f, pos_filho, "No cheio (filho)");
    imprime_no(f, pos_pai, "Pai");

    TARVBMS_divide_no_interno(f, pos_filho, pos_pai, 2);

    printf("\nDepois:\n");
    imprime_no(f, pos_filho, "No esquerdo (filho original)");
    imprime_no(f, pos_pai, "Pai");

    // Le pos do novo no direito a partir do pai
    TConfig cfg; rewind(f); fread(&cfg, sizeof(TConfig), 1, f);
    TARVBMSDADOS info_pai; char ids_p[3][TAM_NOME]; long filhos_p[4]; char arq_p[4][TAM_ARQ_FOLHA];
    le_no_interno(f, pos_pai, &info_pai, ids_p, filhos_p, arq_p);
    imprime_no(f, filhos_p[1], "No direito (novo)");

    fclose(f);
    remove("teste_divide.bin");
}

// Caso 2: no cheio com filhos internos, pai vazio
void teste_caso2()
{
    printf("\n========================================\n");
    printf("CASO 2: no cheio (filho_folha=0), pai vazio\n");
    printf("========================================\n");

    FILE *f = inicializa_arquivo(2);

    // 4 filhos fictícios (posicoes inventadas)
    long filhos_falsos[4] = {100, 200, 300, 400};
    char chaves[3][TAM_NOME] = {"D", "E", "F"};
    long pos_filho = cria_no_interno(f, chaves, filhos_falsos, 3);

    char chaves_pai[3][TAM_NOME] = {"", "", ""};
    long filhos_pai[4] = {pos_filho, -1, -1, -1};
    long pos_pai = cria_no_interno(f, chaves_pai, filhos_pai, 0);

    printf("Antes:\n");
    imprime_no(f, pos_filho, "No cheio (filho)");
    imprime_no(f, pos_pai, "Pai");

    TARVBMS_divide_no_interno(f, pos_filho, pos_pai, 2);

    printf("\nDepois:\n");
    imprime_no(f, pos_filho, "No esquerdo");
    imprime_no(f, pos_pai, "Pai");

    TARVBMSDADOS info_pai; char ids_p[3][TAM_NOME]; long filhos_p[4]; char arq_p[4][TAM_ARQ_FOLHA];
    le_no_interno(f, pos_pai, &info_pai, ids_p, filhos_p, arq_p);
    imprime_no(f, filhos_p[1], "No direito (novo)");

    fclose(f);
    remove("teste_divide.bin");
}

// Caso 3: pai já tem chaves, filho no meio
void teste_caso3()
{
    printf("\n========================================\n");
    printf("CASO 3: pai com chaves existentes, filho no meio\n");
    printf("========================================\n");

    FILE *f = inicializa_arquivo(2);

    // No cheio que vai ser dividido (filho do meio do pai)
    long filhos_falsos[4] = {10, 20, 30, 40};
    char chaves_filho[3][TAM_NOME] = {"M", "N", "O"};
    long pos_filho = cria_no_interno(f, chaves_filho, filhos_falsos, 3);

    // Outros filhos do pai (posicoes fictícias)
    long pos_esq = 999, pos_dir = 888;

    // Pai com 2 chaves: [G, P], filhos: [pos_esq, pos_filho, pos_dir]
    char chaves_pai[3][TAM_NOME] = {"G", "P", ""};
    long filhos_pai[4] = {pos_esq, pos_filho, pos_dir, -1};
    long pos_pai = cria_no_interno(f, chaves_pai, filhos_pai, 2);

    printf("Antes:\n");
    imprime_no(f, pos_filho, "No cheio (filho do meio)");
    imprime_no(f, pos_pai, "Pai");

    TARVBMS_divide_no_interno(f, pos_filho, pos_pai, 2);

    printf("\nDepois:\n");
    imprime_no(f, pos_filho, "No esquerdo");
    imprime_no(f, pos_pai, "Pai");

    TARVBMSDADOS info_pai; char ids_p[3][TAM_NOME]; long filhos_p[4]; char arq_p[4][TAM_ARQ_FOLHA];
    le_no_interno(f, pos_pai, &info_pai, ids_p, filhos_p, arq_p);
    // Novo no fica em filhos_p[2] (entre N e P)
    imprime_no(f, filhos_p[2], "No direito (novo)");

    fclose(f);
    remove("teste_divide.bin");
}

int main()
{
    teste_caso1();
    teste_caso2();
    teste_caso3();
    printf("\nTodos os testes concluidos.\n");
    return 0;
}