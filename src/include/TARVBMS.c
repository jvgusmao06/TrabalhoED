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

TFOLHA *TARVBMS_le_folha(int indice_folha, int t)
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

TFOLHA* TARVBMS_le_folha_por_nome(char nome_folha[30], int t){
    //FALTA TUDO
}

void TARVBMS_imprime_folha(TFOLHA *folha)
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

void TARVBMS_le_no_interno(FILE *f, long posicao, TARVBMSDADOS *infos, char (*ids)[100], long *filhos, char (*arq_folha)[30]){
    fseek(f, posicao, SEEK_SET);

    fread(infos, sizeof(TARVBMSDADOS), 1, f);

    fread(ids, sizeof(char) * 100, infos->nchaves, f);

    if (infos->filho_folha)
    {
        fread(arq_folha, sizeof(char) * 30, infos->nchaves + 1, f);
    }
    else
    {
        fread(filhos, sizeof(long), infos->nchaves + 1, f);
    }
}

long TARVBMS_busca_index_invalido(){
    FILE *f = fopen("../dados/index.bin", "rb");
    if(!f){
        printf("Erro ao abrir o arquivo para busca_index_invalido\n");
        exit(1);
    }

    TConfig c;
    if(fread(&c, sizeof(TConfig), 1, f) != 1 || c.indice <= 1){
        printf("O arquivo está vazio\n");
        fclose(f);
        return -1;
    } 
    
    int max_chaves = (c.t * 2) - 1;
    
    TARVBMSDADOS infos;
    int uniao = max_chaves * TAM_NOME;
    uniao += (max_chaves + 1) * TAM_ARQ_FOLHA;
    
    long pos = ftell(f);
    
    while(fread(&infos, sizeof(TARVBMSDADOS), 1, f) == 1){
        if(infos.val == 0){
            fclose(f);
            return pos;
        }
        fseek(f, uniao, SEEK_CUR);

        pos = ftell(f);
    }
    fclose(f);
    return -1;
}

long TARVBMS_escreve_no(TARVBMS *no){
    FILE *f = fopen("../dados/index.bin", "rb+");
    if(!f){
        printf("Erro ao abrir arquivo para escreve_no");
        exit(1);
    }

    TConfig c;
    fread(&c, sizeof(TConfig), 1, f);
    int max_chaves = (c.t * 2) - 1;
    
    long pos = TARVBMS_busca_index_invalido();

    fseek(f, pos, SEEK_SET);
    
    fwrite(&no->infos, sizeof(TARVBMSDADOS), 1, f);
    fwrite(no->ids, TAM_NOME, max_chaves, f);
    if(no->infos.filho_folha){
        fwrite(no->dados.arq_folha, TAM_ARQ_FOLHA, max_chaves + 1, f);
    }
    else{
        fwrite(no->dados.filhos, TAM_ARQ_FOLHA, max_chaves + 1, f);
    }
    return pos;
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

TFOLHA *TARVBMS_carrega_folha(char *nome){
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

long TARVBMS_raiz(FILE *arq){
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

void TARVBMS_insere_um_do_arquivotxt(char *nome, int t){
    if (TARVBMS_busca(nome))
    {
        printf("Essa entidade ja esta na arvore!\n");
        return;
    }
    FILE *fn = fopen("../../entrada/Nodes.txt", "r");
    if (!fn)
    {
        printf("Erro ao ler o arquivo nodes!\n");
        exit(1);
    }
    TD dado;
    int achou = 0;
    while (ler_entidade(fn, &dado))
    {
        if (dado.tipo == TIPO_FILME)
        {
            if (!strcmp(dado.dados.filme.titulo, nome))
            {
                achou = 1;
                break;
            }
        }
        else if (dado.tipo == TIPO_PESSOA)
        {
            if (!strcmp(dado.dados.pessoa.nome, nome))
            {
                achou = 1;
                break;
            }
        }
    }
    fclose(fn);
    if (!achou)
    {
        printf("Nao existe essa entidade no nodes.txt!");
        return;
    }
    // TARVBMS_insere_um(&dado);
}

void TARVBMS_divide_no_interno(FILE *f, long pos, int t){
    if(!f){
        printf("Não foi passado um arquivo valido para o divide_no_interno");
        exit(1);
    }

    int max_chaves = (2 * t) - 1;
    TARVBMSDADOS infos;
    char ids[max_chaves][100];
    long filhos[max_chaves +  1];
    char arq_folha[max_chaves + 1][TAM_ARQ_FOLHA];

    TARVBMS_le_no_interno(f, pos, &infos, ids, filhos, arq_folha);

    int meio = max_chaves / 2;


}

void TARVBMS_insere_nao_completo(FILE *f, long pos_no, TD *entidade, int t)
{
    int max_chaves = (2 * t) - 1;
    TARVBMSDADOS infos;
    char ids[max_chaves][100];
    long filhos[max_chaves + 1];
    char arq_folha[max_chaves + 1][30];

    TARVBMS_le_no_interno(f, pos_no, &infos, ids, filhos, arq_folha); // Le o no interno e poe no info
    char chave[100];                                                  // Pega a chave da entidade a gnt qr inserir
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
            // TARVBMS_divisao_folha(f, pos_no, i, indice_folha, t);
            //  Como a folha foi dividida, uma nova chave fantasia subiu para o nó atual
            //  O nó atual mudou Precisamos reler os seus dados para atualizar o nosso vetor de chaves em memória.
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
    }
    else
    {
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
        printf("Nao ha entidade! Funcao insere_um TARVBMS");
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
        // printf("%s", entidade->dados.filme.titulo);
    }
    else if (entidade->tipo == TIPO_PESSOA)
    {

        strcpy(chave, entidade->dados.pessoa.nome);
        // printf("%s WE GO THIS \n", entidade->dados.pessoa.nome);
    }

    if (config.indice == 0)
    { // Avore esta vazia
        // Cria a primeira folha em memória principal e insere a entidade
        TFOLHA *nova_folha = TARVBMS_aloca_folha(t);
        nova_folha->entidades[0] = *entidade;
        nova_folha->dados.nchaves = 1;
        strcpy(nova_folha->dados.prox, "-1");
        // printf("%s FAAAAH\n", nova_folha->entidades[0].dados.pessoa.nome);

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
    else if (config.raiz == -1 && config.indice > 0)
    { // Caso em que a da p inserir na folha 0

        char arq[30];
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
            TARVBMS_insere_folha(entidade, t, 0);
        }
        else
        { // A folha ta cheia precisamo do split
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

void TARVBMS_divisao_folha(FILE *f, long pos_no, int i_filho, int indice_folha){
    TConfig config;
    fseek(f, 0, SEEK_SET);
    fread(&config, sizeof(TConfig), 1, f);
    int t = config.t;
    int tam_max = (2 * t) - 1;
    int indice_nova = config.indice;
    TFOLHA* folha_velha = TARVBMS_le_folha(indice_folha, config.t); 
        if(!folha_velha){
            printf("Erro ao ler folha_%d! Funcao TARVBMS_divisao_folha\n", indice_folha);
            exit(1);
        } 
        
    if(pos_no == -1){ //PAI NAO EXISTE -> RAIZ NAO EXISTE
        

        TFOLHA* folha_nova = TARVBMS_aloca_folha(t); 
        int i, n_nova = t;
        int n_velha = t-1;
        config.indice++;

    

        for(i=0; i < n_nova; i++)
            folha_nova->entidades[i] = folha_velha->entidades[t-1 + i];  //Preenche folha nova com infos a partir de t-1

        folha_nova->dados.nchaves = n_nova; //Atualiza numero de chaves das folhas
        folha_velha->dados.nchaves = n_velha;

        strcpy(folha_nova->dados.prox, folha_velha->dados.prox); //Conserta encadeamento
        char nome_nova[TAM_NOME_ARQ];
        sprintf(nome_nova, "folha_%d.bin", indice_nova);
        strcpy(folha_velha->dados.prox, nome_nova);

        TARVBMS_cria_folha(folha_velha, t, indice_folha); //Cria folha nova e reescreve a velha
        TARVBMS_cria_folha(folha_nova, t, indice_nova);

        char chave_promovida[TAM_NOME];
        if(folha_nova->entidades[0].tipo == TIPO_PESSOA)
            strcpy(chave_promovida, folha_nova->entidades[0].dados.pessoa.nome);
        else
            strcpy(chave_promovida, folha_nova->entidades[0].dados.filme.titulo);
    
        //Seta raiz
        TARVBMSDADOS nova_raiz;
        nova_raiz.nchaves = 1;
        nova_raiz.val = 1;
        nova_raiz.filho_folha = 1;

        char ids[1][TAM_NOME];
        strcpy(ids[0], chave_promovida); //Passa valor da primeira chave da direita pra chave da raiz

        char arq_folha[2][TAM_ARQ_FOLHA];
        sprintf(arq_folha[0], "folha_%d.bin", indice_folha); //esquerda = folha velha
        sprintf(arq_folha[1], "folha_%d.bin", indice_nova); // direita = folha nova

        //Arualiza ponteiro do index.bin e escreve raiz nova
        long pos_raiz = sizeof(TConfig);
        fseek(f, pos_raiz, SEEK_SET);

        fwrite(&nova_raiz, sizeof(TARVBMSDADOS), 1, f);
        fwrite(&ids, sizeof(char) * TAM_NOME, 1, f);
        fwrite(&arq_folha,sizeof(char) * TAM_ARQ_FOLHA, 2, f);

        //Atualiza o Tconfig
        config.raiz = pos_raiz;
        fseek(f, 0, SEEK_SET);
        fwrite(&config, sizeof(TConfig), 1, f);
        
        free(folha_velha->entidades);
        free(folha_velha);
        free(folha_nova->entidades);
        free(folha_nova);
        return;
    }

    //LER NO INTERNO E VERIFICAR 
    TARVBMS* pai;
    
    TARVBMS_le_no_interno(f, pos_no, pai, pai->ids, pai->dados.filhos, pai->dados.arq_folha);
    TFOLHA* irmao_dir = TARVBMS_le_folha_por_nome(folha_velha->dados.prox, t);
    if(irmao_dir){

    //PAI EXISTE E NAO ESTA CHEIO
        //VERIFICAR SE IRMAOS ESTAO CHEIOS
        //NAO!!
            //ESQUERDA LIVRE
                //ATUALIZA CHAVE DO PAI[INDICE_FILHO - 1] COM PRIMEIRA CHAVE DO FILHO
                //REESCREVE FOLHA TIRANDO PRIMEIRA CHAVE
                //REESCREVE FOLHA ESQUERDA ACRESCENTANDO A CHAVE NO FINAL
                //SE INDICE CHAVE DO PAI ATUALIZADO FOR = 0, ATUALIZA PAI DO PAI PRA NAO PERDER A ARVORE

            if(irmao_dir->dados.nchaves < tam_max){  //DIREITA LIVRE
                int indice_ultimo = folha_velha->dados.nchaves - 1;
                char chave_promovida[TAM_NOME];
                for(int i = 0; i<pai->infos.nchaves; i++){
                    if(folha_velha->entidades[indice_ultimo].tipo == TIPO_PESSOA){
                        if(strcmp(folha_velha->entidades[0].dados.pessoa.nome, pai->ids[i]) < 0){
                            strcpy(chave_promovida, folha_velha->entidades[indice_ultimo].dados.pessoa.nome);
                            break;
                        }
                    }
                    else if(folha_velha->entidades[0].tipo == TIPO_FILME){
                        if(strcmp(folha_velha->entidades[indice_ultimo].dados.filme.titulo, pai->ids[i]) < 0){
                            strcpy(chave_promovida, folha_velha->entidades[indice_ultimo].dados.filme.titulo);
                            break;
                        }
                    }
                }        
                TARVBMS_remove_da_folha(indice_folha, chave_promovida, t);
            }
            pai->ids[i];
    }
                //ATUALIZA A CHAVE DO PAI[INDICE_FILHO] COM ULTIMA CHAVE DO FILHO
                //REESCREVE FOLHA TIRANDO ULTIMA CHAVE
                //REESCREVE FOLHA DIREITA ACRESCENTANDO CHAVE NO INICIO
                //se n chaves do pai = 1, atualiza pai do pai

        //SIM!!
            //CRIA OUTRO NO FOLHA
            //NO FOLHA NOVO = CHAVES A PARTIR DE [T-1]
            //NO FOLHA VELHO = CHAVES ATE [T-1]
            //REESCREVE NOS FOLHA
            //PROMOVE CHAVE PRO PAI
            //REESCREVE PAI
    
    return;
}


void TARVBMS_removezada(FILE *f, long pos_no, char*chave, int t){
    int max_chaves = (2*t)-1;
    int min_chaves = t-1;
    TARVBMSDADOS info;
    char vet_chaves[max_chaves][TAM_NOME];
    long filhos[max_chaves+1];
    char arq_folha[max_chaves+1][TAM_ARQ_FOLHA];
    

    // Le o no interno atual
    TARVBMS_le_no_interno(f, pos_no,&info,vet_chaves,filhos,arq_folha);

    int i = 0;
    while(i<info.nchaves && strcmp(chave, vet_chaves[i]) >= 0) i++;


    if(info.filho_folha == 1){
        //Fihlo e folha
        int indice_folha_alvo;
        sscanf(arq_folha[i], "folha_%d.bin", &indice_folha_alvo);
        int sucesso = TARVBMS_remove_da_folha(indice_folha_alvo, chave, t);
        // Ve se consegue remover da folha
        if(sucesso){
            TFOLHA *folha_alvo = TARVBMS_le_folha(indice_folha_alvo, t);
            // O registo foi apagado! Agora precisamos verificar se a folha ta belezinha!
            if(folha_alvo->dados.nchaves < min_chaves) // Folha ta ruim, precisamos pegar emprestado
            {
                printf("A folha %d vai precisar pegar emprestado!\n", indice_folha_alvo);


                //FALTA FAZER AQ!!!!! Tentar Empréstimo do irmão da esquerda (i - 1) ou direita (i + 1)
                


                // Se não der para emprestar, fazer Fusão com irmão da esquerda ou direita
            
            }
            free(folha_alvo->entidades);
            free(folha_alvo);
        }
        

    }
    else{
        // FIlho e no interno
        long pos_filho = filhos[i];
        TARVBMS_retira_recursivo(f, pos_filho, chave, t);
        // Fazer a logica dos nores interno
        
    }


}


int TARVBMS_remove_da_folha(int indice_folha, char *chave, int t){
    TFOLHA *folha = TARVBMS_le_folha(indice_folha, t);
    if (!folha) {
        printf("Erro ao ler folha_%d para remocao.\n", indice_folha);
        return 0;
    }
    int i = 0;
    int encontra = 0;
    for(i = 0; i < folha->dados.nchaves; i++){
        char chave_atual[100]; // Acha a chave q vai remover
        if(folha->entidades[i].tipo == TIPO_PESSOA) {
            strcpy(chave_atual, folha->entidades[i].dados.pessoa.nome);
        } else {
            strcpy(chave_atual, folha->entidades[i].dados.filme.titulo);
        }

        if (strcmp(chave_atual, chave) == 0) {
            encontra = 1;
            break; 
        }

        
    }
    if(!encontra){ // Se n encontrou na folha
        intf("A chave '%s' nao foi encontrada na folha %d.\n", chave, indice_folha);
        free(folha->entidades);
        free(folha);
        return 0;
    }
    for (int j=0; j< folha->dados.nchaves-1; j++){
        folha->entidades[j] = folha->entidades[j+1]; // Reorganiza o vetor de entidades;
    }
    folha->dados.nchaves--;
    TARVBMS_cria_folha(folha, t, indice_folha);
    free(folha->entidades);
    free(folha);
    printf("Chave '%s' removida com sucesso da folha %d!\n", chave, indice_folha);
    return 1;
    
    
}


void TARVBMS_retira(char *chave){
    FILE *f = fopen("../dados/index.bin", "rb+");
    if (!f)
    {
        printf("Erro na abertura do arquivo, TARBBMS_retira!");
        exit(1);
    }
    TConfig config;
    rewind(f);
    fread(&config,sizeof(TConfig),1,f);
    int t = config.t;
    if (config.indice == 0){
        printf("Arvore vazia nao ha nada para retirar!\n");
        fclose(f);
        return;

    }
    if (config.raiz == -1 && config.indice>0){ // Caso que so tem a folha.
        TARVBMS_remove_da_folha(0, chave, t);
        fclose(f);
        return;
    }
    TARVBMS_removezada(f, config.raiz, chave, t);


    fclose(f);
}