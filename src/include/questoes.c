#include "../header/questoes.h"

void insere_par(char *a, char *b, int *npares, int *cap_pares, Par **pares)
{
    char *pa = (strcasecmp(a, b) < 0) ? a : b;
    char *pb = (strcasecmp(a, b) < 0) ? b : a;


    for (int i = 0; i < *npares; i++) {
        if (strcasecmp((*pares)[i].pessoaA, pa) == 0 &&
            strcasecmp((*pares)[i].pessoaB, pb) == 0) {
            (*pares)[i].count++;
            return;
        }
    }


    if (*npares >= *cap_pares) {
        int nova_cap = (*cap_pares == 0) ? 64 : (*cap_pares * 2);

        Par *tmp = realloc(*pares, nova_cap * sizeof(Par));
        if (!tmp) {
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

void mais_atuaram()
{
    Frequencia *vet_freq = NULL;
    int npessoas = 0;

    FILE *fhash = fopen(arqHashPessoa, "rb");
    if (!fhash)
        exit(1);
    int offset_hash;
    THP aux;
    for (offset_hash = 0; offset_hash < TAM_HASH; offset_hash++)
    {

        fread(&aux, sizeof(int), 1, fhash);
        if (aux.offset != -1)
        {
        }
    }
}