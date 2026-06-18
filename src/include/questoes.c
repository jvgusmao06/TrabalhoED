#include "../header/questoes.h"

void insere_par(char *a, char *b, int *npares, int *cap_pares, Par **pares)
{
    char *pa = (strcasecmp(a, b) < 0) ? a : b;
    char *pb = (strcasecmp(a, b) < 0) ? b : a;

    for (int i = 0; i < *npares; i++)
    {
        if (strcasecmp((*pares)[i].pessoaA, pa) == 0 &&
            strcasecmp((*pares)[i].pessoaB, pb) == 0)
        {
            (*pares)[i].count++;
            return;
        }
    }

    if (*npares >= *cap_pares)
    {
        int nova_cap = (*cap_pares == 0) ? 64 : (*cap_pares * 2);

        Par *tmp = realloc(*pares, nova_cap * sizeof(Par));
        if (!tmp)
        {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

        *pares = tmp;
        *cap_pares = nova_cap;
    }

    strcpy((*pares)[*npares].pessoaA, pa);
    strcpy((*pares)[*npares].pessoaB, pb);
    (*pares)[*npares].count = 1;

    (*npares)++;
}

void insere_freq(Frequencia **vet_freq, char *pessoa, int *n)
{
    for (int i = 0; i < *n; i++)
    {
        if (strcasecmp((*vet_freq)[i].pessoa, pessoa) == 0)
        {
            (*vet_freq)[i].count++;
            return;
        }
    }
    strcpy((*vet_freq)[*n].pessoa, pessoa);
    (*vet_freq)[*n].count = 1;
    (*n)++;
}

void troca(Frequencia *a, Frequencia *b)
{
    Frequencia tmp = *a;
    *a = *b;
    *b = tmp;
}

int particiona_rev(Frequencia *vet_freq, int low, int high)
{
    int pivot = vet_freq[high].count;
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (vet_freq[j].count >= pivot)
        {
            i++;
            troca(&vet_freq[i], &vet_freq[j]);
        }
    }
    troca(&vet_freq[i + 1], &vet_freq[high]);
    return i + 1;
}

void qs_freq_rev(Frequencia *vet_freq, int tam_preenchido)
{
    if (tam_preenchido <= 1)
        return;
    int pi = particiona_rev(vet_freq, 0, tam_preenchido - 1);
    qs_freq_rev(vet_freq, pi);
    qs_freq_rev(vet_freq + pi + 1, tam_preenchido - pi - 1);
}

int particiona(Frequencia *vet_freq, int low, int high)
{
    int pivot = vet_freq[high].count;
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (vet_freq[j].count <= pivot)
        {
            i++;
            troca(&vet_freq[i], &vet_freq[j]);
        }
    }
    troca(&vet_freq[i + 1], &vet_freq[high]);
    return i + 1;
}

void qs_freq(Frequencia *vet_freq, int tam_preenchido)
{
    if (tam_preenchido <= 1)
        return;
    int pi = particiona(vet_freq, 0, tam_preenchido - 1);
    qs_freq(vet_freq, pi);
    qs_freq(vet_freq + pi + 1, tam_preenchido - pi - 1);
}

void QA()
{
    Par *pares = NULL;
    int npares = 0;
    int cap_pares = 0;

    FILE *fhash = fopen(arqHashFilme, "rb");
    FILE *fdados = fopen(arqDado, "rb");
    if (!fhash || !fdados)
        exit(1);

    THF aux;
    int c = 0;
    for (int i = 0; i < TAM_HASH; i++)
    {

        fseek(fhash, i * sizeof(THF), SEEK_SET);
        fread(&aux, sizeof(THF), 1, fhash);
        if (aux.offset == -1)
            continue;

        char pessoas[200][50];
        int n = 0;
        TR reg;
        int offset = aux.offset;
        while (offset != -1)
        {
            fseek(fdados, offset, SEEK_SET);
            fread(&reg, sizeof(TR), 1, fdados);
            if (reg.valido)
                strcpy(pessoas[n++], reg.nome_pessoa);
            offset = reg.THFprox;
        }

        for (int a = 0; a < n; a++)
            for (int b = a + 1; b < n; b++)
            {
                if (strcasecmp(pessoas[a], pessoas[b]) != 0)
                {
                    c++;
                    printf("%d. %s <-> %s\n", c, pessoas[a], pessoas[b]);
                    insere_par(pessoas[a], pessoas[b], &npares, &cap_pares, &pares);
                }
            }
    }
    fclose(fhash);
    fclose(fdados);

    for (int i = 0; i < npares; i++)
        printf("%s <-> %s | %d vez(es)\n",
               pares[i].pessoaA, pares[i].pessoaB, pares[i].count);

    free(pares);
    pares = NULL;
    npares = 0;
    cap_pares = 0;
}

void QB()
{
    Par *pares = NULL;
    int npares = 0;
    int cap_pares = 0;

    FILE *fhash = fopen(arqHashFilme, "rb");
    FILE *fdados = fopen(arqDado, "rb");
    if (!fhash || !fdados)
        exit(1);

    THF aux;
    for (int i = 0; i < TAM_HASH; i++)
    {
        fseek(fhash, i * sizeof(THF), SEEK_SET);
        fread(&aux, sizeof(THF), 1, fhash);
        if (aux.offset == -1)
            continue;

        char pessoas[100][50];
        int n = 0;
        TR reg;
        int offset = aux.offset;
        while (offset != -1)
        {
            fseek(fdados, offset, SEEK_SET);
            fread(&reg, sizeof(TR), 1, fdados);
            if (reg.valido && (reg.tipo == ACTED_IN || reg.tipo == DIRECTED))
                strcpy(pessoas[n++], reg.nome_pessoa);
            offset = reg.THFprox;
        }

        for (int a = 0; a < n; a++)
            for (int b = a + 1; b < n; b++)
            {
                if (strcasecmp(pessoas[a], pessoas[b]) != 0)
                    insere_par(pessoas[a], pessoas[b], &npares, &cap_pares, &pares);
            }
    }
    fclose(fhash);
    fclose(fdados);

    for (int i = 0; i < npares; i++)
        printf("%s <-> %s | %d vez(es)\n",
               pares[i].pessoaA, pares[i].pessoaB, pares[i].count);

    free(pares);
    pares = NULL;
    npares = 0;
    cap_pares = 0;
}

void QC()
{
    Par *pares = NULL;
    int npares = 0;
    int cap_pares = 0;

    FILE *fhash = fopen(arqHashFilme, "rb");
    FILE *fdados = fopen(arqDado, "rb");
    if (!fhash || !fdados)
        exit(1);

    THF aux;
    for (int i = 0; i < TAM_HASH; i++)
    {
        fseek(fhash, i * sizeof(THF), SEEK_SET);
        fread(&aux, sizeof(THF), 1, fhash);
        if (aux.offset == -1)
            continue;

        char pessoas[200][50];
        int n = 0;
        TR reg;
        int offset = aux.offset;
        while (offset != -1)
        {
            fseek(fdados, offset, SEEK_SET);
            fread(&reg, sizeof(TR), 1, fdados);
            if (reg.valido && reg.tipo == ACTED_IN)
                strcpy(pessoas[n++], reg.nome_pessoa);
            offset = reg.THFprox;
        }

        for (int a = 0; a < n; a++)
            for (int b = a + 1; b < n; b++)
            {
                if (strcasecmp(pessoas[a], pessoas[b]) != 0)
                    insere_par(pessoas[a], pessoas[b], &npares, &cap_pares, &pares);
            }
    }
    fclose(fhash);
    fclose(fdados);

    for (int i = 0; i < npares; i++)
        printf("%s <-> %s | %d vez(es)\n",
               pares[i].pessoaA, pares[i].pessoaB, pares[i].count);

    free(pares);
    pares = NULL;
    npares = 0;
    cap_pares = 0;
}

void QF()
{
    Frequencia *vet_freq = (Frequencia *)malloc(sizeof(Frequencia) * 200);
    int npessoas = 0;

    FILE *fhash = fopen(arqHashPessoa, "rb");
    if (!fhash)
        exit(1);
    int offset_hash;
    THP aux;
    TR aux2;
    FILE *fdados = fopen(arqDado, "rb");
    for (offset_hash = 0; offset_hash < TAM_HASH; offset_hash++)
    {
        fseek(fhash, offset_hash * sizeof(THP), SEEK_SET);
        fread(&aux, sizeof(THP), 1, fhash);

        while (aux.offset != -1)
        {
            fseek(fdados, aux.offset, SEEK_SET);
            fread(&aux2, sizeof(TR), 1, fdados);
            if (aux2.tipo == ACTED_IN)
                insere_freq(&vet_freq, aux.nome_pessoa, &npessoas);
            aux.offset = aux2.THPprox;
        }
    }
    qs_freq_rev(vet_freq, npessoas);
    printf("================================================================================================================================================\n");
    printf("Atores que mais atuaram, com lista de filmes\n");
    for (int i = 0; i < 3; i++)
    {
        TR_imprime_relacoes_pessoa(vet_freq[i].pessoa);
    }
    free(vet_freq);
    fclose(fhash);
    fclose(fdados);
}

void QG()
{
    Frequencia *vet_freq = (Frequencia *)malloc(sizeof(Frequencia) * 200);
    int npessoas = 0;

    FILE *fhash = fopen(arqHashPessoa, "rb");
    if (!fhash)
        exit(1);
    int offset_hash;
    THP aux;
    TR aux2;
    FILE *fdados = fopen(arqDado, "rb");
    for (offset_hash = 0; offset_hash < TAM_HASH; offset_hash++)
    {
        fseek(fhash, offset_hash * sizeof(THP), SEEK_SET);
        fread(&aux, sizeof(THP), 1, fhash);

        while (aux.offset != -1)
        {
            fseek(fdados, aux.offset, SEEK_SET);
            fread(&aux2, sizeof(TR), 1, fdados);
            if (aux2.tipo == ACTED_IN)
                insere_freq(&vet_freq, aux2.nome_pessoa, &npessoas);
            aux.offset = aux2.THPprox;
        }
    }
    qs_freq(vet_freq, npessoas);
    printf("================================================================================================================================================\n");
    printf("Atores que menos atuaram, com lista de filmes\n");
    for (int i = 0; i < 3; i++)
    {
        TR_imprime_relacoes_pessoa(vet_freq[i].pessoa);
    }
    free(vet_freq);
    fclose(fhash);
    fclose(fdados);
}

int index_da_primeira_folha(int *t)
{
    FILE *f = fopen("../dados/index.bin", "rb");
    if (!f)
    {
        printf("Erro ao abrir o arquivo de indice para questao t\n");
        exit(1);
    }

    TConfig config;
    fread(&config, sizeof(TConfig), 1, f);
    *t = config.t;

    if (config.indice == 0)
    {
        printf("Arvore vazia\n");
        fclose(f);
        return -1;
    }

    int indice_folha;
    if (config.raiz == -1)
    {
        fclose(f);
        return 0;
    }

    int max_chaves = *t * 2 - 1;
    int pos = config.raiz;
    TARVBMSDADOS info;
    char ids[max_chaves][TAM_NOME];
    long filhos[max_chaves + 1];
    char arq_folha[max_chaves + 1][TAM_ARQ_FOLHA];

    while (1)
    {
        le_no_interno(f, pos, &info, ids, filhos, arq_folha);
        if (info.filho_folha)
        {
            sscanf(arq_folha[0], "folha_%d.bin", &indice_folha);
            break;
        }
        pos = filhos[0];
    }

    fclose(f);
    return indice_folha;
}

int eh_artigo(const char *palavra)
{
    const char *artigos[] = {"o", "a", "os", "as", "um", "uma", "the", "an"};
    int n = sizeof(artigos) / sizeof(artigos[0]);

    for (int i = 0; i < n; i++)
    {
        if (strcasecmp(palavra, artigos[i]) == 0)
            return 1;
    }
    return 0;
}

void QM()
{
    int t;
    int ind = index_da_primeira_folha(&t);
    int capacidade = 16, n_filmes = 0;
    char (*titulos)[TAM_NOME] = malloc(capacidade * sizeof(*titulos));

    int pos = ind;

    while (1)
    {
        TFOLHA *folha = TFOLHA_le(pos, t);
        if (!folha)
            break;

        for (int j = 0; j < folha->dados.nchaves; j++)
        {
            if (folha->entidades[j].tipo == TIPO_FILME)
            {
                if (n_filmes == capacidade)
                {
                    capacidade *= 2;
                    titulos = realloc(titulos, capacidade * sizeof(*titulos));
                }
                strcpy(titulos[n_filmes], folha->entidades[j].dados.filme.titulo);
                n_filmes++;
            }
        }
        char prox[TAM_ARQ_FOLHA];
        strcpy(prox, folha->dados.prox);

        free(folha->entidades);
        free(folha);

        if (strcmp(prox, "-1") == 0)
            break;
        sscanf(prox, "folha_%d.bin", &pos);
    }
    char (*primeira_palavras)[TAM_NOME] = malloc(n_filmes * sizeof(*primeira_palavras));

    for (int i = 0; i < n_filmes; i++)
    {
        char *p = titulos[i];
        int k = 0;

        while (*p != '\0' && *p != ' ')
        {
            primeira_palavras[i][k] = *p++;
            k++;
        }
        primeira_palavras[i][k] = '\0';

        if (eh_artigo(primeira_palavras[i]) && *p == ' ')
        {
            p++;
            k = 0;
            while (*p != '\0' && *p != ' ')
            {
                primeira_palavras[i][k] = *p++;
                k++;
            }
            primeira_palavras[i][k] = '\0';
        }
    }

    int *eh_continuacao = calloc(n_filmes, sizeof(int));
    for (int i = 0; i < n_filmes - 1; i++)
    {
        if (strcasecmp(primeira_palavras[i], primeira_palavras[i + 1]) == 0)
        {
            eh_continuacao[i] = 1;
            eh_continuacao[i + 1] = 1;
        }
    }

    printf("================================================================================================================================================\n");
    printf("Filmes que sao continuacoes\n");
    int achou = 0;
    for (int i = 0; i < n_filmes; i++)
    {
        if (eh_continuacao[i])
        {
            printf("%s\n", titulos[i]);
            achou = 1;
        }
    }
    if (!achou)
        printf("Nenhum filme encontrado.\n");
    free(titulos);
    free(primeira_palavras);
    free(eh_continuacao);
}

void QT()
{
    int t;
    int folha_inicial = index_da_primeira_folha(&t);
    if (folha_inicial == -1)
        return;

    int capacidade = 16, n_anos = 0;
    int *anos_filme = (int *)malloc(capacidade * sizeof(int));
    int ind = folha_inicial;
    while (1)
    {
        TFOLHA *folha = TFOLHA_le(ind, t);
        if (!folha)
            break;

        for (int j = 0; j < folha->dados.nchaves; j++)
        {
            if (folha->entidades[j].tipo == TIPO_FILME)
            {
                if (n_anos == capacidade)
                {
                    capacidade *= 2;
                    anos_filme = realloc(anos_filme, capacidade * sizeof(int));
                }
                anos_filme[n_anos] = folha->entidades[j].dados.filme.ano;
                n_anos++;
            }
        }
        char prox[TAM_ARQ_FOLHA];
        strcpy(prox, folha->dados.prox);
        free(folha->entidades);
        free(folha);

        if (strcmp(prox, "-1") == 0)
            break;
        sscanf(prox, "folha_%d.bin", &ind);
    }
    printf("================================================================================================================================================\n");
    printf("Atores nascidos no ano de lançamento de algum filme\n");
    int achou = 0;
    ind = folha_inicial;

    while (1)
    {
        TFOLHA *folha = TFOLHA_le(ind, t);
        if (!folha)
            break;

        for (int j = 0; j < folha->dados.nchaves; j++)
        {
            if (folha->entidades[j].tipo == TIPO_PESSOA)
            {
                int ano_nasc = folha->entidades[j].dados.pessoa.ano;
                for (int k = 0; k < n_anos; k++)
                {
                    if (anos_filme[k] == ano_nasc)
                    {
                        printf("%s nasceu em %d\n", folha->entidades[j].dados.pessoa.nome, ano_nasc);
                        achou = 1;
                        break;
                    }
                }
            }
        }
        char prox[TAM_ARQ_FOLHA];
        strcpy(prox, folha->dados.prox);
        free(folha->entidades);
        free(folha);
        if (strcmp(prox, "-1") == 0)
        break;
        sscanf(prox, "folha_%d.bin", &ind);
    }
    if (!achou)
    {
        printf("Nenhum ator encontrado\n");
    }
    free(anos_filme);
}

void QH()
{
    Frequencia *vet_freq = (Frequencia *)malloc(sizeof(Frequencia) * 200);
    int npessoas = 0;

    FILE *fhash = fopen(arqHashPessoa, "rb");
    if (!fhash)
        exit(1);
    int offset_hash;
    THP aux;
    TR aux2;
    FILE *fdados = fopen(arqDado, "rb");
    for (offset_hash = 0; offset_hash < TAM_HASH; offset_hash++)
    {
        fseek(fhash, offset_hash * sizeof(THP), SEEK_SET);
        fread(&aux, sizeof(THP), 1, fhash);

        while (aux.offset != -1)
        {
            fseek(fdados, aux.offset, SEEK_SET);
            fread(&aux2, sizeof(TR), 1, fdados);
            if (aux2.tipo == DIRECTED)
                insere_freq(&vet_freq, aux.nome_pessoa, &npessoas);
            aux.offset = aux2.THPprox;
        }
    }
    qs_freq_rev(vet_freq, npessoas);
    for (int i = 0; i < 3; i++)
    {
        TR_imprime_relacoes_pessoa(vet_freq[i].pessoa);
    }
    free(vet_freq);
    fclose(fhash);
    fclose(fdados);
}

void QI()
{
    Frequencia *vet_freq = (Frequencia *)malloc(sizeof(Frequencia) * 200);
    int npessoas = 0;

    FILE *fhash = fopen(arqHashPessoa, "rb");
    if (!fhash)
        exit(1);
    int offset_hash;
    THP aux;
    TR aux2;
    FILE *fdados = fopen(arqDado, "rb");
    for (offset_hash = 0; offset_hash < TAM_HASH; offset_hash++)
    {
        fseek(fhash, offset_hash * sizeof(THP), SEEK_SET);
        fread(&aux, sizeof(THP), 1, fhash);

        while (aux.offset != -1)
        {
            fseek(fdados, aux.offset, SEEK_SET);
            fread(&aux2, sizeof(TR), 1, fdados);
            if (aux2.tipo == DIRECTED)
                insere_freq(&vet_freq, aux.nome_pessoa, &npessoas);
            aux.offset = aux2.THPprox;
        }
    }
    qs_freq(vet_freq, npessoas);
    for (int i = 0; i < 3; i++)
    {
        TR_imprime_relacoes_pessoa(vet_freq[i].pessoa);
    }
    free(vet_freq);
    fclose(fhash);
    fclose(fdados);
}

void QJ()
{
    Frequencia *vet_freq = (Frequencia *)malloc(sizeof(Frequencia) * 200);
    int npessoas = 0;

    FILE *fhash = fopen(arqHashPessoa, "rb");
    if (!fhash)
        exit(1);
    int offset_hash;
    THP aux;
    TR aux2;
    FILE *fdados = fopen(arqDado, "rb");
    for (offset_hash = 0; offset_hash < TAM_HASH; offset_hash++)
    {
        fseek(fhash, offset_hash * sizeof(THP), SEEK_SET);
        fread(&aux, sizeof(THP), 1, fhash);

        while (aux.offset != -1)
        {
            fseek(fdados, aux.offset, SEEK_SET);
            fread(&aux2, sizeof(TR), 1, fdados);
            if (aux2.tipo == PRODUCED)
                insere_freq(&vet_freq, aux.nome_pessoa, &npessoas);
            aux.offset = aux2.THPprox;
        }
    }
    qs_freq_rev(vet_freq, npessoas);
    for (int i = 0; i < 3; i++)
    {
        TR_imprime_relacoes_pessoa(vet_freq[i].pessoa);
    }
    free(vet_freq);
    fclose(fhash);
    fclose(fdados);
}

void QK()
{
    Frequencia *vet_freq = (Frequencia *)malloc(sizeof(Frequencia) * 200);
    int npessoas = 0;

    FILE *fhash = fopen(arqHashPessoa, "rb");
    if (!fhash)
        exit(1);
    int offset_hash;
    THP aux;
    TR aux2;
    FILE *fdados = fopen(arqDado, "rb");
    for (offset_hash = 0; offset_hash < TAM_HASH; offset_hash++)
    {
        fseek(fhash, offset_hash * sizeof(THP), SEEK_SET);
        fread(&aux, sizeof(THP), 1, fhash);

        while (aux.offset != -1)
        {
            fseek(fdados, aux.offset, SEEK_SET);
            fread(&aux2, sizeof(TR), 1, fdados);
            if (aux2.tipo == PRODUCED)
                insere_freq(&vet_freq, aux.nome_pessoa, &npessoas);
            aux.offset = aux2.THPprox;
        }
    }
    qs_freq(vet_freq, npessoas);
    for (int i = 0; i < 3; i++)
    {
        TR_imprime_relacoes_pessoa(vet_freq[i].pessoa);
    }
    free(vet_freq);
    fclose(fhash);
    fclose(fdados);
}

int TR_busca_pessoa_arquivo(FILE* fhash, char *pessoa)
{
    int offset_arqHashPessoa = hashit(pessoa) * sizeof(THP);
    int offset_dado;
    THP hashNode;
    fseek(fhash, offset_arqHashPessoa, SEEK_SET);
    fread(&hashNode, sizeof(THP), 1, fhash);
    offset_dado = hashNode.offset;
    if ((offset_dado == -1) || (strcasecmp(hashNode.nome_pessoa, pessoa) != 0))
    {
        return -1;
    }
    return offset_dado;
}

int TR_busca_pra_mim(FILE* fhash, FILE *fdados, char *nome_pessoa, TipoRelacao tipo)
{
    int offset = TR_busca_pessoa_arquivo(fhash, nome_pessoa);
    if (offset == -1)
        return -1;

    TR aux;
    while (offset != -1)
    {
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        if (aux.tipo == tipo)
            if (aux.valido == 1)
            {
                return 1;
            }
        offset = aux.THPprox;
    }
    return -1;
}

void QO()
{
    FILE *fhash = fopen(arqHashPessoa, "rb");
    if (!fhash)
        exit(1);
    TR aux;
    THP aux_hash;
    FILE *fdados = fopen(arqDado, "rb");
    if (!fdados)
        exit(1);
    for (int i = 0; i < TAM_HASH; i++)
    {
        fseek(fhash, i * sizeof(THP), SEEK_SET);
        fread(&aux_hash, sizeof(THP), 1, fhash);
        if (TR_busca_pra_mim(fhash, fdados, aux_hash.nome_pessoa, ACTED_IN) == 1 
        && TR_busca_pra_mim(fhash, fdados, aux_hash.nome_pessoa, DIRECTED) == 1)
            printf("%s\n", aux_hash.nome_pessoa);
    }
    fclose(fdados);
    fclose(fhash);
}

void QP()
{
    FILE *fhash = fopen(arqHashPessoa, "rb");
    if (!fhash)
        exit(1);
    TR aux;
    THP aux_hash;
    FILE *fdados = fopen(arqDado, "rb");
    if (!fdados)
        exit(1);
    for (int i = 0; i < TAM_HASH; i++)
    {
        fseek(fhash, i * sizeof(THP), SEEK_SET);
        fread(&aux_hash, sizeof(THP), 1, fhash);
        if (TR_busca_pra_mim(fhash, fdados, aux_hash.nome_pessoa, ACTED_IN) == 1
         && TR_busca_pra_mim(fhash, fdados, aux_hash.nome_pessoa, PRODUCED) == 1)
            printf("%s\n", aux_hash.nome_pessoa);
    }
    fclose(fdados);
    fclose(fhash);
}



















typedef struct {
    char nome[100];
    int ano;
} Ator;

int comparar_por_ano(const void *a, const void *b) {
    return (((Ator*)a)->ano - ((Ator*)b)->ano);
}




void QN()
{
    int t;
    int indice_atual = index_da_primeira_folha(&t);
    char arq[50];
    
    int total_atores = 0;
    int capacidade = 50;
    Ator *lista = malloc(capacidade * sizeof(Ator)); 

    while (indice_atual != -1) {
        TFOLHA *folha = TFOLHA_le(indice_atual, t); 
        if (!folha) break;

        // Processa os atores desta folha
       for (int i = 0; i < folha->dados.nchaves; i++) {
        if (folha->entidades[i].tipo == TIPO_PESSOA) {
                
                // Expande memória se necessário
                if (total_atores >= capacidade) {
                    capacidade *= 2;
                    lista = realloc(lista, capacidade * sizeof(Ator));
                }
                strcpy(lista[total_atores].nome, folha->entidades[i].dados.pessoa.nome);
                lista[total_atores].ano = folha->entidades[i].dados.pessoa.ano;
                total_atores++;

            }
        }

        char prox_arquivo[50];
        strcpy(prox_arquivo, folha->dados.prox);
        
        free(folha->entidades);
        free(folha);

        if (strcmp(prox_arquivo, "-1") == 0) {
            indice_atual = -1;
        } else {
            sscanf(prox_arquivo, "folha_%d.bin", &indice_atual);
        }
        qsort(lista, total_atores, sizeof(Ator), comparar_por_ano);

    }

    //Imprimir
    printf("\n=== Questao (n): Atores que nasceram no mesmo ano ===\n");
    int ano_atual = -1;
    for (int i = 0; i < total_atores; i++) {
        if (lista[i].ano != ano_atual) {
            ano_atual = lista[i].ano;
            printf("\n--- Nascidos em %d ---\n", ano_atual);
        }
        printf(" - %s\n", lista[i].nome);
    }

    free(lista);
    printf("\n===================================================67\n");

}