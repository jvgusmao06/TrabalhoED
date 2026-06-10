#include "../header/TARVBMS.h"
#include <stdlib.h>
#include <string.h>

void TARVBMS_inicializa(int t)
{ // Cria todos os arquivos vazios
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

TFOLHA *TARVBMS_aloca_folha(int t)
{
    TFOLHA *novo = (TFOLHA *)malloc(sizeof(TFOLHA));
    int tam = (t * 2) - 1;
    novo->entidades = (TD *)malloc(sizeof(TD) * tam);
    novo->dados.nchaves = 1;
    return novo;
}

void TARVBMS_cria_folha(TFOLHA *folha, int t, int index)
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

int comparar_por_nome(TD *e1, TD *e2)
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

void TARVBMS_insere_folha(TD *entidade, int t, int index)
{
    char arq[30];
    sprintf(arq, "../dados/folha_%d.bin", index);

    FILE *f = fopen(arq, "rb+");

    TFOLHA *folha = NULL;

    if (!f)
    {
        printf("A folha ../dados/folha_%d.bin não existe. Criando uma folha.\n", index);

        folha = TARVBMS_aloca_folha(t);

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

/*
void TARVBMS_ordena_folha(int index){
    char arq[30];
    sprintf(arq, "../../dados/folha_%d.bin", index);

    FILE *f = fopen(arq, "rb+");
    if(!f){
        printf("Não foi possível abrir o arquivo folha_%d.bin para ordenação", index);
        exit(1);
        }

        TDADOSFOLHA dados;
        fread(&dados, sizeof(TDADOSFOLHA), 1, f);

        if(dados.nchaves <= 1){
            fclose
            }
            int tam_max = sizeof(TDADOSFOLHA) + dados.nchaves * sizeof(TD);
            int i;
            TD ent;
            while(i< tam_max){

            }
            }
            */

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
/*
void TARVBMS_insere_um_do_arquivotxt(char *nome, int t)
{

if(TARVBMS_busca(nome, t)){
    printf("Essa entidade ja esta na arvore!\n");
    return;
    }
    FILE *fn = fopen("../../entrada/Nodes.txt", "r");
    if(!fn){
        printf("Erro ao ler o arquivo nodes!\n");
        exit(1);
        }
        TD dado;
        int achou = 0;
        while(ler_entidade(fn, &dado)){
            if(dado.tipo == TIPO_FILME){
                if(!strcmp(dado.dados.filme.titulo, nome)){
                    achou = 1;
                    break;
                    }

                    }else if(dado.tipo == TIPO_PESSOA){
                        if(!strcmp(dado.dados.pessoa.nome, nome)){
                            achou = 1;
                            break;
                            }
                            }
                            }
                            fclose(fn);
                            if(!achou){
                                printf("Nao existe essa entidade no nodes.txt!");
                                return;
                                }
                                //TARVBMS_insere_um(&dado);

                                }
                                */
void TARVBMS_le_no_interno(FILE *f, long posicao, TARVBMSDADOS *infos, char (*ids)[100], long *filhos, char (*arq_folha)[30])

{
    fseek(f, posicao, SEEK_SET);

    fread(infos, sizeof(TARVBMSDADOS), 1, f);

    fread(ids, sizeof(char) * 100, infos->nchaves, f);

    if (infos->val == 1)
    {
        fread(arq_folha, sizeof(char) * 30, infos->nchaves + 1, f);
    }
    else
    {
        fread(filhos, sizeof(long), infos->nchaves + 1, f);
    }
}

//void TARVBMS_divisao_folha(); Vai ser implementado



void TARVBMS_insere_nao_completo(FILE *f, long pos_no, TD *entidade, int t)
{
    int max_chaves = (2 * t) - 1;
    TARVBMSDADOS infos;
    char ids[max_chaves][100];
    long filhos[max_chaves + 1];
    char arq_folha[max_chaves + 1][30];

    TARVBMS_le_no_interno(f, pos_no, &infos, ids, filhos, arq_folha); // Le o no interno e poe no info
    char chave[100]; // Pega a chave da entidade a gnt qr inserir
    if (entidade->tipo == TIPO_PESSOA) {
        strcpy(chave, entidade->dados.pessoa.nome);
    } else {
        strcpy(chave, entidade->dados.filme.titulo);
    }


    int i = infos.nchaves - 1;
    while (i >= 0 && strcmp(chave, ids[i]) < 0) i--; // Logica rossetiana para achar o indice correto onde ele deve ser inserido
    i++;  // i fica com a pos exata do filho

    if (infos.filho_folha == 1) // Caso A: Os filho e folha
    {
        int indice_folha;
        sscanf(arq_folha[i], "folha_%d.bin", &indice_folha); // Extrai o indice da folha


        char arq_filho[30]; // Abrindo a folha p ver se ta cheia
        sprintf(arq_filho, "../dados/folha_%d.bin", indice_folha);
        FILE *ffolha = fopen(arq_filho, "rb");
        if (!ffolha) {
            printf("Erro ao abrir %s. Funcao insere nao completo, if_filho folha == 1 \n", arq_filho);
            exit(1);
        }
        TDADOSFOLHA dados_folha;
        fread(&dados_folha, sizeof(TDADOSFOLHA), 1, ffolha);
        fclose(ffolha);

        if(dados_folha.nchaves >= max_chaves){ // Folha esta cheia, precisa do split
            printf("A folha %d esta cheia. Comecando divisao\n", indice_folha);
            //TARVBMS_divisao_folha(f, pos_no, i, indice_folha, t);
            // Como a folha foi dividida, uma nova chave fantasia subiu para o nó atual
            // O nó atual mudou! Precisamos reler os seus dados para atualizar o nosso vetor de chaves em memória.
            TARVBMS_le_no_interno(f, pos_no, &infos, ids, filhos, arq_folha);



            if (strcmp(chave, ids[i]) > 0) // Verificacao p ver se a chave que queremos inserir e maior que a chave que acabou de subir
            {
                i++;
                // Atualizamos o indice_folha para o número da nova folha
                sscanf(arq_folha[i], "folha_%d.bin", &indice_folha);
            }
        }

        
        printf("Inserindo chave '%s' na folha %d!\n", chave, indice_folha);
        TARVBMS_insere_folha(entidade, t, indice_folha);
    }else{
        // CASO B: OS FILHOS SÃO NÓS INTERNOS
        long pos_filho = filhos[i];
        // Falta a logica aq
        printf("Descendo para o nó interno na posicao %ld\n", pos_filho);
        TARVBMS_insere_nao_completo(f, pos_filho, entidade, t);
    }
}


void TARVBMS_insere_um(TD *entidade)
{
    if (!entidade)
    {
        printf("Nao ha entidade! Funcao insere_um TARBMS");
        return;
    }
    FILE *f = fopen("../dados/index.bin", "rb+");
    if (!f)
    {
        printf("Erro na abertura do arquivo, TARBBMS_insere_um!");
        exit(1);
    }
    TConfig config;
    fseek(f, 0, SEEK_SET);
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;
    int max_chaves = (2 * t) - 1;

    char chave[100];
    if (entidade->tipo == TIPO_FILME)
    {
        strcpy(chave, entidade->dados.filme.titulo);
        //printf("%s", entidade->dados.filme.titulo);
    }
    else if (entidade->tipo == TIPO_PESSOA)
    {

        strcpy(chave, entidade->dados.pessoa.nome);
        //printf("%s WE GO THIS \n", entidade->dados.pessoa.nome);
    }

    if (config.indice == 0)
    { // Avore esta vazia
        // Cria a primeira folha em memória principal e insere a entidade
        TFOLHA *nova_folha = TARVBMS_aloca_folha(t);
        nova_folha->entidades[0] = *entidade;
        nova_folha->dados.nchaves = 1;
        strcpy(nova_folha->dados.prox, "-1");
        //printf("%s FAAAAH\n", nova_folha->entidades[0].dados.pessoa.nome);

        // Grava a folha no seu arquivo binário distinto (folha_0.bin)
        TARVBMS_cria_folha(nova_folha, t, config.indice);
        config.indice = 1; // Incrementa, pois agora temos a folha_0

        fseek(f, 0, SEEK_SET);
        fwrite(&config, sizeof(TConfig), 1, f);
        
        free(nova_folha->entidades);
        free(nova_folha);
        fclose(f);
        return;
    }
        else if(config.raiz == -1 && config.indice>0){ // Caso em que a da p inserir na folha 0

        char arq[30];
        sprintf(arq, "../dados/folha_0.bin");
        FILE *ffolha = fopen(arq, "rb");
        
        if (!ffolha) {
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
            TARVBMS_insere_folha(entidade, t, 0); 
        }else{// A folha ta cheia precisamo do split


        }
    }
    else
    {
        TARVBMSDADOS info_raiz;
        fseek(f, config.raiz, SEEK_SET);
        fread(&info_raiz, sizeof(TARVBMSDADOS), 1, f);
        if (info_raiz.nchaves == max_chaves)
        {
            printf("A raiz esta cheia. Iniciar logica de SPLIT de raiz!\n");
        }
        else
        {
            printf("Descendo na arvore\n");
            TARVBMS_insere_nao_completo(f, config.raiz, entidade, t);
        }
        fclose(f);
    }
}

void TARVBMS_imprime_folha(TFOLHA *folha)
{
    if (!folha) return;

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

TFOLHA *TARVBMS_busca(char *nome)
{
    FILE *f = fopen("../dados/index.bin", "rb");
    if (!f)
    {
        printf("Erro ao abrir o arquivo ../dados/index.bin para busca\n");
        exit(1);
    }

    TConfig c;
    fread(&c, sizeof(TConfig), 1, f);
    
    if (c.indice <= 0 || c.raiz == -1){
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

        for(int i = 0; i < folha->dados.nchaves; i++){
            if(folha->entidades[i].tipo == TIPO_PESSOA){
                if(strcmp(nome, folha->entidades[i].dados.pessoa.nome) == 0){
                    fclose(f);
                    return folha;
                }
            }
            else{
                if(strcmp(nome, folha->entidades[i].dados.filme.titulo) == 0){
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

        if(no_infos.nchaves <= 0 || no_infos.nchaves > max_chaves){
            fclose(f);
            return NULL;
        }
        
        int i = 0;
        while (i < no_infos.nchaves && strcmp(nome, ids[i]) > 0) i++;

        if (no_infos.val == 1)
        {
            char nome_arquivo_folha[30];
            strcpy(nome_arquivo_folha, arq_folha[i]);

            fclose(f);

            char caminho_folha[60];
            sprintf(caminho_folha, "../dados/%s", nome_arquivo_folha);

            FILE *ffolha = fopen(caminho_folha, "rb");
            if (!ffolha) return NULL;

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
