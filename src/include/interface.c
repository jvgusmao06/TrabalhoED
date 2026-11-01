#include "../header/interface.h"
#include "../header/TD.h"
#include "../header/TARVBMS.h"
#include "../header/TFOLHA.h"
#include "../header/questoes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int le_inteiro(char *mensagem){
    int valido;
    char buffer[10];
    int valor;

    printf("%s", mensagem);
    do{
        valido = 1;
        scanf(" %9[^\n]", buffer);
        
        for(int i = 0; buffer[i]!= '\0'; i++){
            if(i == 0 && buffer[i] == '-'){
                continue;
            }

            if(!isdigit(buffer[i])){
                valido = 0;
                break;
            }
        }
        if(!valido){
            printf("Entrada invalida! Digite um numero valido:\n");
        }

    }while(!valido);
    valor = atoi(buffer);
    return valor;
}

int escolhe_t()
{
    int t = le_inteiro("Digite o t da arvore:\n");
    if(t < 2) t = 2;
    return t;
}

void insere_entidades()
{
    char buffer[10];
    int tipo;
    while (1)
    {
        printf("================================================================================================================================================\n");
        tipo = le_inteiro("Digite o tipo da sua entidade:\n1: para pessoa\n2: para filme\n-1: para parar de inserir\n");

        if (tipo == -1)
        {
            return;
        }
        else if (tipo == 1)
        {

            TD *entidade = (TD *)malloc(sizeof(TD));
            entidade->tipo = TIPO_PESSOA;
            char nome[TAM_NOME];
            int ano;
            printf("Digite o nome da pessoa:\n");
            scanf(" %99[^\n]", nome);
            ano = le_inteiro("Digite o ano de nascimento\n");

            strcpy(entidade->dados.pessoa.nome, nome);
            entidade->dados.pessoa.ano = ano;
            TARVBMS_insere(entidade);
            free(entidade);
        }
        else if (tipo == 2)
        {
            TD *entidade = (TD *)malloc(sizeof(TD));
            entidade->tipo = TIPO_FILME;
            char nome[TAM_NOME], sub[TAM_SUB];
            int ano;
            printf("Digite o nome do filme:\n");
            scanf(" %99[^\n]", nome);
            printf("Digite o ano de lançamento\n");
            scanf("%d", &ano);
            printf("Digite o subtitulo do filme:\n");
            scanf(" %199[^\n]", sub);

            strcpy(entidade->dados.filme.titulo, nome);
            entidade->dados.filme.ano = ano;
            strcpy(entidade->dados.filme.sub, sub);
            TARVBMS_insere(entidade);
            free(entidade);
        }
        else{
            printf("Tipo invalido.\n");
        }
    }
}


void remove_entidades()
{
    char nome[TAM_NOME];
    while (1)
    {
        printf("================================================================================================================================================\n");
        printf("Digite o nome ou o titulo de uma entidade para remover ou -1 para parar de remover\n");
        scanf(" %99[^\n]", nome);

        if(strcmp(nome, "-1") == 0){
            return;
        }
        TARVBMS_retira(nome);
    }
}

void busca_entidades()
{
    char nome[TAM_NOME];
    while (1)
    {
        printf("================================================================================================================================================\n");
        printf("Digite o nome da pessoa (ou titulo do filme) para buscar na folha ou -1 para voltar:\n");
        scanf(" %99[^\n]", nome);

        if (strcmp(nome, "-1") == 0)
        {
            return;
        }

        TFOLHA *folha = TFOLHA_carrega(nome);
        if (!folha)
        {
            printf("Entidade \"%s\" nao encontrada na arvore!\n", nome);
            continue;
        }

        printf("\n--- Folha Encontrada ---\n");
        TFOLHA_imprime(folha);

        for (int i = 0; i < folha->dados.nchaves; i++)
        {
            if (folha->entidades[i].tipo == TIPO_PESSOA &&
                strcasecmp(folha->entidades[i].dados.pessoa.nome, nome) == 0)
            {
                printf("\n=== Dados da Pessoa ===\n");
                printf("Nome: %s\n", folha->entidades[i].dados.pessoa.nome);
                printf("Ano de nascimento: %d\n", folha->entidades[i].dados.pessoa.ano);
                break;
            }
            else if (folha->entidades[i].tipo == TIPO_FILME &&
                     strcasecmp(folha->entidades[i].dados.filme.titulo, nome) == 0)
            {
                printf("\n=== Dados do Filme ===\n");
                printf("Titulo: %s\n", folha->entidades[i].dados.filme.titulo);
                printf("Ano de lancamento: %d\n", folha->entidades[i].dados.filme.ano);
                printf("Subtitulo: %s\n", folha->entidades[i].dados.filme.sub);
                break;
            }
        }

        free(folha->entidades);
        free(folha);
    }
}

void questoes()
{
    char buffer[10];
    printf("================================================================================================================================================\n");
    printf("Digite a letra da questao que voce quer ou -1 para sair de questoes:\n");
    scanf(" %9[^\n]", buffer);

    if(strcasecmp(buffer, "-1") == 0) return;

    else if(strcasecmp(buffer, "a") == 0) QA();

    else if(strcasecmp(buffer, "b") == 0) QB();

    else if(strcasecmp(buffer, "c") == 0) QC();

    else if(strcasecmp(buffer, "f") == 0) QF();

    else if(strcasecmp(buffer, "g") == 0) QG();

    else if(strcasecmp(buffer, "h") == 0) QH();

    else if(strcasecmp(buffer, "i") == 0) QI();

    else if(strcasecmp(buffer, "j") == 0) QJ();

    else if(strcasecmp(buffer, "k") == 0) QK();

    else if(strcasecmp(buffer, "m") == 0) QM();

    else if(strcasecmp(buffer, "o") == 0) QO();

    else if(strcasecmp(buffer, "p") == 0) QP();

    else if(strcasecmp(buffer, "t") == 0) QT();

    else{
        printf("Questao nao exite ou nao fizemos essa questao:\n");
        questoes();
    } 
}
