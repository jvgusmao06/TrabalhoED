#include "../header/interface.h"
#include "../header/TD.h"
#include "../header/TARVBMS.h"
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
            printf("Entrada inválida! Digite um numero valido:\n");
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
        printf("Digite o nome ou o título de uma entidade para remover ou -1 para parar de remover\n");
        scanf(" %99[^\n]", nome);

        if(strcmp(nome, "-1") == 0){
            return;
        }
        TARVBMS_retira(nome);
    }
}

void questoes()
{
    return;
}