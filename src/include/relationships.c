#include "../header/relationships.h"

int hashit(char *nome)
{
    trim(nome);
    char copia[100];
    strncpy(copia, nome, sizeof(copia) - 1);
    copia[99] = '\0';

    char *str = copia;
    for (int i = 0; str[i] != '\0'; i++)
        str[i] = tolower(str[i]);

    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % TAM_HASH;
}

void push_livre(int offset)
{
    FILE *f = fopen(arqPosLivre, "ab");
    if (!f)
        exit(1);
    fwrite(&offset, sizeof(int), 1, f);
    printf("DEBUG: offset livre %d\n", offset);
    fclose(f);
}

int pop_livre()
{
    FILE *flivres = fopen(arqPosLivre, "rb+");
    if (!flivres)
        return -1;

    fseek(flivres, 0, SEEK_END);
    long tamanho = ftell(flivres);
    if (tamanho < sizeof(int))
    {
        fclose(flivres);
        return -1;
    }

    int offset;
    fseek(flivres, -sizeof(int), SEEK_END);
    fread(&offset, sizeof(int), 1, flivres);

    fseek(flivres, tamanho - sizeof(int), SEEK_SET);
    fclose(flivres);

    flivres = fopen(arqPosLivre, "ab");
    fclose(flivres);
    truncate(arqPosLivre, tamanho - sizeof(int));

    return offset;
}

void trim(char *str)
{

    int inicio = 0;
    while (str[inicio] == ' ')
        inicio++;

    int i = 0;
    while (str[inicio + i] != '\0')
    {
        str[i] = str[inicio + i];
        i++;
    }
    str[i] = '\0';

    int fim = strlen(str) - 1;
    while (fim >= 0 && str[fim] == ' ')
        str[fim--] = '\0';
}

void preenche_hashes(char *txt)
{
    FILE *fteste = fopen(arqDado, "rb");
    fseek(fteste, 0, SEEK_END);
    if (ftell(fteste) > sizeof(TR))
        return; // Arquivo ja preenchido com Relationships.txt
    fclose(fteste);
    FILE *finput = fopen(txt, "r");
    if (!finput)
        exit(1);

    char linha[256];
    while (fgets(linha, sizeof(linha), finput))
    {
        // START Person | Meg Ryan | ACTED_IN | END Movie | When Harry Met Sally | role: Sally Albright
        linha[strcspn(linha, "\n")] = '\0';
        char *token = strtok(linha, "|"); // "START Person "
        if (!token)
            continue;

        token = strtok(NULL, "|"); // " Nome da Pessoa "
        if (!token)
            continue;
        trim(token);
        char nome[50];
        strncpy(nome, token, sizeof(nome) - 1);

        token = strtok(NULL, "|"); // " TIPO_REL "
        if (!token)
            continue;
        trim(token);
        TipoRelacao tipo;
        if (strcmp(token, "ACTED_IN") == 0)
            tipo = ACTED_IN;
        else if (strcmp(token, "DIRECTED") == 0)
            tipo = DIRECTED;
        else if (strcmp(token, "PRODUCED") == 0)
            tipo = PRODUCED;
        else
            tipo = WROTE;

        strtok(NULL, "|"); // " END Movie "
        if (!token)
            continue;

        token = strtok(NULL, "|"); // " Nome Do Filme "
        if (!token)
            continue;
        trim(token);
        char filme[100];
        strncpy(filme, token, sizeof(filme) - 1);
        if (tipo == ACTED_IN)
        {
            token = strtok(NULL, "|"); // " role: Papel 1, Papel 2, Papel 3"
            if (!token)
                continue;
            trim(token);
            char *dois_pontos = strchr(token, ':');
            if (!dois_pontos)
                continue;
            char *papeis = dois_pontos + 2;
            if (!papeis)
                continue;

            char *papel = strtok(papeis, ","); // " Papel 2 "
            if (!papel)
                continue;
            while (papel != NULL)
            {
                if (!papel)
                    continue;
                trim(papel);

                TR elem;
                elem.valido = 1;
                strcpy(elem.nome_pessoa, nome);
                strcpy(elem.papel, papel);
                strcpy(elem.nome_filme, filme);
                elem.tipo = tipo;
                elem.THFprox = -1;
                elem.THPprox = -1;

                THF_insere(&elem);
                THP_insere(&elem);
                papel = strtok(NULL, ",");
            }
        }
        else
        {
            TR elem;
            elem.valido = 1;
            elem.THFprox = -1;
            elem.THPprox = -1;
            elem.tipo = tipo;
            elem.papel[0] = '\0';
            strcpy(elem.nome_pessoa, nome);
            strcpy(elem.nome_filme, filme);

            THF_insere(&elem);
            THP_insere(&elem);
        }
    }
    fclose(finput);
}

void TH_inicializa()
{
    FILE *teste = fopen(arqHashFilme, "rb");
    if (teste)
    {
        fclose(teste);
        return;
    }
    FILE *fhashp = fopen(arqHashPessoa, "wb");
    FILE *fhashf = fopen(arqHashFilme, "wb");
    FILE *fdados = fopen(arqDado, "wb");
    FILE *flivres = fopen(arqPosLivre, "wb");

    if ((!fhashf || !fdados) || !flivres)
    {
        printf("Não foi possível criar os arquivos da hash\n");
        exit(1);
    }
    fclose(fdados);
    fclose(flivres);

    int i;
    int vazio = -1;
    char nome_vazio[] = "\0";
    THF hashNode;
    THP hashNodep;
    strcpy(hashNode.filme, nome_vazio);
    hashNode.offset = vazio;
    strcpy(hashNodep.nome_pessoa, nome_vazio);
    hashNodep.offset = vazio;
    for (i = 0; i < TAM_HASH; i++)
    {
        fwrite(&hashNode, sizeof(THF), 1, fhashf); // preenche arquivo criado com -1 e filme vazio
        fwrite(&hashNodep, sizeof(THP), 1, fhashp);
    }
    fclose(fhashf);
    fclose(fhashp);
}

void THP_insere(TR *elem)
{

    int a = TR_busca_endereco_por_pessoa(elem->nome_filme, elem->nome_pessoa, elem->tipo, elem->papel);
    if (a != -1)
        return;

    int offset_elem = TR_busca_endereco(elem->nome_filme, elem->nome_pessoa, elem->tipo, elem->papel);
    if (offset_elem == -1)
        return;

    int offset_hash = TR_busca_pessoa(elem->nome_pessoa);
    if (offset_hash == -1)
    {
        offset_hash = hashit(elem->nome_pessoa) * sizeof(THP);
        FILE *fhash = fopen(arqHashPessoa, "rb+");
        fseek(fhash, offset_hash, SEEK_SET);
        THP aux;
        strcpy(aux.nome_pessoa, elem->nome_pessoa);
        aux.offset = offset_elem;
        fwrite(&aux, sizeof(THP), 1, fhash);
        fclose(fhash);
        return;
    }

    FILE *fdados = fopen(arqDado, "rb+");
    TR aux;
    int offset_ant = -1;
    while (offset_hash != -1)
    {
        fseek(fdados, offset_hash, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        offset_ant = offset_hash;
        offset_hash = aux.THPprox;
    }
    aux.THPprox = offset_elem;
    fseek(fdados, offset_ant, SEEK_SET);
    fwrite(&aux, sizeof(TR), 1, fdados);
    fclose(fdados);
    return;
}

void THF_insere(TR *elem)
{
    int a = TR_busca_filme(elem->nome_filme);
    if (a == -1)
    { // Filme nao existe na tabela
        FILE *fhash = fopen(arqHashFilme, "rb+");
        FILE *fdados = fopen(arqDado, "rb+");
        if (!fhash || !fdados)
            exit(1);
        int offset_arqHashFilme = hashit(elem->nome_filme) * sizeof(THF);

        int novo_offset = pop_livre();
        if (novo_offset == -1)
        {
            fseek(fdados, 0, SEEK_END);
            novo_offset = ftell(fdados);
        }
        THF hashNode;
        strcpy(hashNode.filme, elem->nome_filme);
        hashNode.offset = novo_offset;
        fseek(fdados, novo_offset, SEEK_SET);
        elem->THFprox = -1;
        elem->THPprox = -1;
        elem->valido = 1;
        fwrite(elem, sizeof(TR), 1, fdados);
        fseek(fhash, offset_arqHashFilme, SEEK_SET);
        fwrite(&hashNode, sizeof(THF), 1, fhash);
        fclose(fhash);
        fclose(fdados);
        return;
    }

    int offset_elemento = TR_busca_endereco(elem->nome_filme, elem->nome_pessoa, elem->tipo, elem->papel);
    if (offset_elemento != -1)
        return;
    FILE *fdados = fopen(arqDado, "rb+");
    if (!fdados)
        exit(1);

    TR atual;
    fseek(fdados, a, SEEK_SET);
    fread(&atual, sizeof(TR), 1, fdados);

    while (atual.THFprox != -1)
    {
        a = atual.THFprox;
        fseek(fdados, a, SEEK_SET);
        fread(&atual, sizeof(TR), 1, fdados);
    }

    int novo_offset = pop_livre();
    if (novo_offset == -1)
    {
        fseek(fdados, 0, SEEK_END);
        novo_offset = ftell(fdados);
    }
    atual.THFprox = novo_offset;
    fseek(fdados, novo_offset, SEEK_SET);
    elem->valido = 1;
    elem->THFprox = -1;
    elem->THPprox = -1;
    fwrite(elem, sizeof(TR), 1, fdados);
    fseek(fdados, a, SEEK_SET);
    fwrite(&atual, sizeof(TR), 1, fdados);
    fclose(fdados);
}

int TR_busca_filme(char *filme)
{
    int offset_arqHashFilme = hashit(filme) * sizeof(THF);
    FILE *fhash = fopen(arqHashFilme, "rb");
    if (!fhash)
        exit(1);

    int offset_dado;
    THF hashNode;
    fseek(fhash, offset_arqHashFilme, SEEK_SET);
    fread(&hashNode, sizeof(THF), 1, fhash);
    fclose(fhash);
    offset_dado = hashNode.offset;
    if ((offset_dado == -1) || (strcasecmp(hashNode.filme, filme) != 0))
    {
        return -1;
    }
    return offset_dado;
}

int TR_busca_pessoa(char *pessoa)
{
    int offset_arqHashPessoa = hashit(pessoa) * sizeof(THP);
    FILE *fhash = fopen(arqHashPessoa, "rb");
    if (!fhash)
        exit(1);

    int offset_dado;
    THP hashNode;
    fseek(fhash, offset_arqHashPessoa, SEEK_SET);
    fread(&hashNode, sizeof(THP), 1, fhash);
    fclose(fhash);
    offset_dado = hashNode.offset;
    if ((offset_dado == -1) || (strcasecmp(hashNode.nome_pessoa, pessoa) != 0))
    {
        return -1;
    }
    return offset_dado;
}

int TR_busca_endereco(char *filme, char *nome_pessoa, TipoRelacao tipo, char *papel)
{
    int offset = TR_busca_filme(filme);
    if (offset == -1)
        return -1;

    FILE *fdados = fopen(arqDado, "rb");
    if (!fdados)
        exit(1);

    TR aux;
    while (offset != -1)
    {
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        if (strcasecmp(aux.nome_pessoa, nome_pessoa) == 0 &&
            aux.tipo == tipo &&
            strcasecmp(aux.papel, papel) == 0)
        {
            if (aux.valido == 1)
            {
                fclose(fdados);
                return offset;
            }
            fclose(fdados);
            return -1;
        }
        offset = aux.THFprox;
    }
    fclose(fdados);
    return -1;
}

int TR_busca_endereco_por_pessoa(char *filme, char *nome_pessoa, TipoRelacao tipo, char *papel)
{
    int offset = TR_busca_pessoa(nome_pessoa);
    if (offset == -1)
        return -1;

    FILE *fdados = fopen(arqDado, "rb");
    if (!fdados)
        exit(1);

    TR aux;
    while (offset != -1)
    {
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        if (strcasecmp(aux.nome_filme, filme) == 0 &&
            aux.tipo == tipo &&
            strcasecmp(aux.papel, papel) == 0)
        {
            if (aux.valido == 1)
            {
                fclose(fdados);
                return offset;
            }
            fclose(fdados);
            return -1;
        }
        offset = aux.THPprox;
    }
    fclose(fdados);
    return -1;
}

int TR_busca_filme_anterior(FILE *fdados, char *filme, char *nome_pessoa, TipoRelacao tipo, char *papel)
{
    int offset = TR_busca_filme(filme);

    if (offset == -1)
        return -1;

    int offset_anterior = -1;
    TR aux;

    while (offset != -1)
    {
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);

        if (strcasecmp(aux.nome_pessoa, nome_pessoa) == 0 && (aux.tipo == tipo) && (strcasecmp(aux.papel, papel) == 0) && aux.valido == 1)
        {
            return offset_anterior;
        }
        offset_anterior = offset;
        offset = aux.THFprox;
    }
    return -1;
}

int TR_busca_pessoa_anterior(FILE *fdados, char *filme, char *nome_pessoa, TipoRelacao tipo, char *papel)
{
    int offset = TR_busca_pessoa(nome_pessoa);
    if (offset == -1)
        return -1;

    int offset_anterior = -1;
    TR aux;

    while (offset != -1)
    {
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);

        if (strcasecmp(aux.nome_filme, filme) == 0 && (aux.tipo == tipo) && (strcasecmp(aux.papel, papel) == 0) && aux.valido == 1)
        {
            return offset_anterior;
        }
        offset_anterior = offset;
        offset = aux.THPprox;
    }
    return -1;
}

void TR_retira_relacionamento(FILE *fdados, char *filme, char *nome_pessoa, TipoRelacao tipo, char *papel)
{

    int offset_filme_ant = TR_busca_filme_anterior(fdados, filme, nome_pessoa, tipo, papel);
    int offset_pessoa_ant = TR_busca_pessoa_anterior(fdados, filme, nome_pessoa, tipo, papel);
    int offset_elem = TR_busca_endereco(filme, nome_pessoa, tipo, papel);

    TR aux;
    if (offset_elem == -1)
        return;
    fseek(fdados, offset_elem, SEEK_SET);
    fread(&aux, sizeof(TR), 1, fdados);

    if (offset_filme_ant == -1 && offset_pessoa_ant == -1)
    {
        if (aux.THFprox == -1)
            escreve_hash_filme_vazio(hashit(filme) * sizeof(THF));
        else
            atualiza_root_filme(hashit(filme) * sizeof(THF), aux.THFprox);

        if (aux.THPprox == -1)
            escreve_hash_pessoa_vazio(hashit(nome_pessoa) * sizeof(THP));
        else
            atualiza_root_pessoa(hashit(nome_pessoa) * sizeof(THP), aux.THPprox);
    }

    else if ((offset_filme_ant == -1) && (offset_pessoa_ant != -1))
    {
        TR atualizado;
        fseek(fdados, offset_pessoa_ant, SEEK_SET);
        fread(&atualizado, sizeof(TR), 1, fdados);
        atualizado.THPprox = aux.THPprox;
        fseek(fdados, offset_pessoa_ant, SEEK_SET);
        fwrite(&atualizado, sizeof(TR), 1, fdados);

        if (aux.THFprox == -1)
            escreve_hash_filme_vazio(hashit(filme) * sizeof(THF));
        else
            atualiza_root_filme(hashit(filme) * sizeof(THF), aux.THFprox);
    }
    else if ((offset_filme_ant != -1) && (offset_pessoa_ant == -1))
    {
        TR atualizado;
        fseek(fdados, offset_filme_ant, SEEK_SET);
        fread(&atualizado, sizeof(TR), 1, fdados);
        atualizado.THFprox = aux.THFprox;
        fseek(fdados, offset_filme_ant, SEEK_SET);
        fwrite(&atualizado, sizeof(TR), 1, fdados);

        if (aux.THPprox == -1)
            escreve_hash_pessoa_vazio(hashit(nome_pessoa) * sizeof(THP));
        else
            atualiza_root_pessoa(hashit(nome_pessoa) * sizeof(THP), aux.THPprox);
    }
    else
    {
        TR atualizado;
        fseek(fdados, offset_filme_ant, SEEK_SET);
        fread(&atualizado, sizeof(TR), 1, fdados);
        atualizado.THFprox = aux.THFprox;
        fseek(fdados, offset_filme_ant, SEEK_SET);
        fwrite(&atualizado, sizeof(TR), 1, fdados);

        fseek(fdados, offset_pessoa_ant, SEEK_SET);
        fread(&atualizado, sizeof(TR), 1, fdados);
        atualizado.THPprox = aux.THPprox;
        fseek(fdados, offset_pessoa_ant, SEEK_SET);
        fwrite(&atualizado, sizeof(TR), 1, fdados);
    }

    fseek(fdados, offset_elem, SEEK_SET);
    aux.valido = 0;
    aux.THFprox = -1;
    aux.THPprox = -1;
    fwrite(&aux, sizeof(TR), 1, fdados);
    push_livre(offset_elem);
    int novo_root = TR_busca_filme(filme);
}

void TR_retira_relacao(char *filme, char *nome_pessoa, TipoRelacao tipo, char *papel)
{
    FILE *fdados = fopen(arqDado, "rb+");
    if (!fdados)
        exit(1);
    TR_retira_relacionamento(fdados, filme, nome_pessoa, tipo, papel);
    fclose(fdados);
}

void TR_retira_pessoa(char *pessoa)
{
    int data_offset = TR_busca_pessoa(pessoa);
    if (data_offset == -1)
        return;
    FILE *fdados = fopen(arqDado, "rb+");
    if (!fdados)
        exit(1);
    TR aux;

    while (TR_busca_pessoa(pessoa) != -1)
    {
        data_offset = TR_busca_pessoa(pessoa);
        fseek(fdados, data_offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        TR_retira_relacionamento(fdados, aux.nome_filme, aux.nome_pessoa, aux.tipo, aux.papel);
    }
    fclose(fdados);
}

void TR_retira_filme(char *filme)
{
    int data_offset = TR_busca_filme(filme);
    if (data_offset == -1)
        return;
    FILE *fdados = fopen(arqDado, "rb+");
    if (!fdados)
        exit(1);
    TR aux;

    while (TR_busca_filme(filme) != -1)
    {
        data_offset = TR_busca_filme(filme);
        fseek(fdados, data_offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        TR_retira_relacionamento(fdados, aux.nome_filme, aux.nome_pessoa, aux.tipo, aux.papel);
    }
    fclose(fdados);
}

void TR_imprime_relacoes_filme_pessoa(char *filme, char *pessoa)
{
    int offset = TR_busca_filme(filme);
    if (offset == -1)
    {
        printf("Filme não encontrado!\n");
        return;
    }

    FILE *fdados = fopen(arqDado, "rb");
    if (!fdados)
        exit(1);

    int encontrou = 0;
    TR aux;
    while (offset != -1)
    {
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        if (strcasecmp(aux.nome_pessoa, pessoa) == 0 && aux.valido == 1)
        {
            encontrou = 1;
            printf("Nome: %s | Tipo: %d | Papel: %s\n", aux.nome_pessoa, aux.tipo, aux.papel);
        }
        offset = aux.THFprox;
    }
    if (!encontrou)
        printf("Nenhum relacionamento encontrado!\n");
    fclose(fdados);
}

void TR_imprime_relacoes_filme(char *filme)
{
    int offset = TR_busca_filme(filme);
    if (offset == -1)
    {
        printf("Filme não encontrado!\n");
        return;
    }
    int offset_filme = hashit(filme) * sizeof(THF);
    FILE *fhash = fopen(arqHashFilme, "rb");
    fseek(fhash, offset_filme, SEEK_SET);
    THF hashNode;
    fread(&hashNode, sizeof(THF), 1, fhash);
    strcpy(filme, hashNode.filme);
    fclose(fhash);
    FILE *fdados = fopen(arqDado, "rb");
    if (!fdados)
        exit(1);

    printf("\n=== %s ===\n", filme);
    TR aux;
    while (offset != -1)
    {
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        if (aux.valido == 1)
        {
            char *tipo_str;
            switch (aux.tipo)
            {
            case ACTED_IN:
                tipo_str = "ACTED_IN";
                break;
            case DIRECTED:
                tipo_str = "DIRECTED";
                break;
            case PRODUCED:
                tipo_str = "PRODUCED";
                break;
            case WROTE:
                tipo_str = "WROTE";
                break;
            default:
                tipo_str = "?";
                break;
            }
            printf("  %s | %s | %s\n", aux.nome_pessoa, tipo_str, aux.papel);
        }
        offset = aux.THFprox;
    }
    fclose(fdados);
}

void TR_imprime_relacoes_pessoa(char *pessoa)
{
    int offset = TR_busca_pessoa(pessoa);
    if (offset == -1)
    {
        printf("Pessoa não encontrado!\n");
        return;
    }
    int offset_pessoa = hashit(pessoa) * sizeof(THP);
    FILE *fhash = fopen(arqHashPessoa, "rb");
    fseek(fhash, offset_pessoa, SEEK_SET);
    THP hashNode;
    fread(&hashNode, sizeof(THP), 1, fhash);
    strcpy(pessoa, hashNode.nome_pessoa);
    fclose(fhash);
    FILE *fdados = fopen(arqDado, "rb");
    if (!fdados)
        exit(1);

    printf("\n=== %s ===\n", pessoa);
    TR aux;
    while (offset != -1)
    {
        fseek(fdados, offset, SEEK_SET);
        fread(&aux, sizeof(TR), 1, fdados);
        if (aux.valido == 1)
        {
            char *tipo_str;
            switch (aux.tipo)
            {
            case ACTED_IN:
                tipo_str = "ACTED_IN";
                break;
            case DIRECTED:
                tipo_str = "DIRECTED";
                break;
            case PRODUCED:
                tipo_str = "PRODUCED";
                break;
            case WROTE:
                tipo_str = "WROTE";
                break;
            default:
                tipo_str = "?";
                break;
            }
            printf("  %s | %s | %s\n", tipo_str, aux.nome_filme, aux.papel);
        }
        offset = aux.THPprox;
    }
    fclose(fdados);
}

void TR_menu()
{
    int opcao;
    char filme[100];
    char nome[50];
    char papel[50];
    int tipo;

    do
    {
        printf("\n=== MENU ===\n");
        printf("1. Inserir relacionamento\n");
        printf("2. Buscar relacionamentos de um filme\n");
        printf("3. Buscar relacionamentos de uma pessoa\n");
        printf("4. Buscar relacionamentos de uma pessoa num filme\n");
        printf("5. Remover relacionamento\n");
        printf("6. Remover todos os relacionamentos de um filme\n");
        printf("7. Remover todos os relacionamentos de uma pessoa\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao)
        {
        case 1:
            printf("Filme: ");
            fgets(filme, sizeof(filme), stdin);
            filme[strcspn(filme, "\n")] = '\0';

            printf("Nome da pessoa: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';

            printf("Tipo (0=ACTED_IN, 1=DIRECTED, 2=PRODUCED, 3=WROTE): ");
            scanf("%d", &tipo);
            getchar();

            printf("Papel (enter para vazio): ");
            fgets(papel, sizeof(papel), stdin);
            papel[strcspn(papel, "\n")] = '\0';

            TR elem;
            elem.valido = 1;
            elem.THFprox = -1;
            elem.THPprox = -1;
            elem.tipo = tipo;
            strcpy(elem.nome_pessoa, nome);
            strcpy(elem.nome_filme, filme);
            strcpy(elem.papel, papel);
            THF_insere(&elem);
            THP_insere(&elem);
            printf("Inserido com sucesso!\n");
            break;

        case 2:
            printf("Filme: ");
            fgets(filme, sizeof(filme), stdin);
            filme[strcspn(filme, "\n")] = '\0';
            TR_imprime_relacoes_filme(filme);
            break;

        case 3:
            printf("Nome da pessoa: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';
            TR_imprime_relacoes_pessoa(nome);
            break;

        case 4:
            printf("Filme: ");
            fgets(filme, sizeof(filme), stdin);
            filme[strcspn(filme, "\n")] = '\0';

            printf("Nome da pessoa: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';
            TR_imprime_relacoes_filme_pessoa(filme, nome);
            break;

        case 5:
            printf("Filme: ");
            fgets(filme, sizeof(filme), stdin);
            filme[strcspn(filme, "\n")] = '\0';

            printf("Nome da pessoa: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';

            printf("Tipo (0=ACTED_IN, 1=DIRECTED, 2=PRODUCED, 3=WROTE): ");
            scanf("%d", &tipo);
            getchar();

            printf("Papel (enter para vazio): ");
            fgets(papel, sizeof(papel), stdin);
            papel[strcspn(papel, "\n")] = '\0';

            TR_retira_relacao(filme, nome, tipo, papel);
            printf("Removido com sucesso!\n");
            break;

        case 6:
            printf("Filme: ");
            fgets(filme, sizeof(filme), stdin);
            filme[strcspn(filme, "\n")] = '\0';
            TR_retira_filme(filme);
            printf("Todos os relacionamentos do filme removidos!\n");
            break;

        case 7:
            printf("Nome da pessoa: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';
            TR_retira_pessoa(nome);
            printf("Todos os relacionamentos da pessoa removidos!\n");
            break;

        case 0:
            printf("Saindo...\n");
            break;

        default:
            printf("Opcao invalida!\n");
            break;
        }
    } while (opcao != 0);
}

void escreve_hash_filme_vazio(int offset)
{
    FILE *fhash = fopen(arqHashFilme, "rb+");
    fseek(fhash, offset, SEEK_SET);
    int vazio = -1;
    char nome_vazio[] = "\0";
    THF hashNode;
    strcpy(hashNode.filme, nome_vazio);
    hashNode.offset = vazio;
    fwrite(&hashNode, sizeof(THF), 1, fhash);
    fclose(fhash);
}

void escreve_hash_pessoa_vazio(int offset)
{
    FILE *fhash = fopen(arqHashPessoa, "rb+");
    fseek(fhash, offset, SEEK_SET);
    int vazio = -1;
    char nome_vazio[] = "\0";
    THP hashNode;
    strcpy(hashNode.nome_pessoa, nome_vazio);
    hashNode.offset = vazio;
    fwrite(&hashNode, sizeof(THP), 1, fhash);
    fclose(fhash);
}

void atualiza_root_filme(int offset_movie, int offset_relationships)
{
    FILE *fhash = fopen(arqHashFilme, "rb+");
    fseek(fhash, offset_movie, SEEK_SET);
    THF hashNode;
    fread(&hashNode, sizeof(THF), 1, fhash);
    hashNode.offset = offset_relationships;
    fseek(fhash, offset_movie, SEEK_SET);
    fwrite(&hashNode, sizeof(THF), 1, fhash);
    fclose(fhash);
}

void atualiza_root_pessoa(int offset_pessoa, int offset_relationship)
{
    FILE *fhash = fopen(arqHashPessoa, "rb+");
    fseek(fhash, offset_pessoa, SEEK_SET);
    THP hashNode;
    fread(&hashNode, sizeof(THP), 1, fhash);
    hashNode.offset = offset_relationship;
    fseek(fhash, offset_pessoa, SEEK_SET);
    fwrite(&hashNode, sizeof(THP), 1, fhash);
    fclose(fhash);
}
