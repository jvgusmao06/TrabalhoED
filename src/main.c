#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/TARVBMS.h"
#include "header/util.h"

int main() {
    int t = 2;

    printf("1. Inicializando o indice...\n");
    TARVBMS_inicializa(t);

    // Criando apenas UMA entidade (Caso A)
    TD e1;
    e1.tipo = TIPO_PESSOA; // Use a macro do seu código
    strcpy(e1.dados.pessoa.nome, "Keanu Reeves");
    printf("%s\n", e1.dados.pessoa.nome);
    e1.dados.pessoa.ano = 1964;
    TD e2;
    e2.tipo = TIPO_PESSOA; // Use a macro do seu código
    strcpy(e2.dados.pessoa.nome, "Aini goat");
    printf("%s\n", e2.dados.pessoa.nome);
    e2.dados.pessoa.ano = 1000;

    printf("2. Inserindo o primeiro elemento (Testando o Caso A)...\n");
    TARVBMS_insere_um(&e1);
    TARVBMS_insere_um(&e2);
    printf("3. Verificando o disco...\n");
    printf("O tamanho de TConfig é: %zu bytes\n", sizeof(TConfig));
    // Lendo o index.bin para ver se a raiz foi atualizada
    FILE *f_index = fopen("../dados/index.bin", "rb");
    if (f_index) {
        TConfig config;
        fread(&config, sizeof(TConfig), 1, f_index);
        printf("\n--- Lendo index.bin ---\n");
        printf("Raiz atualizada para a posicao: %ld\n", config.raiz);
        printf("Proximo indice de folha: %d\n", config.indice);
        
        // Vai para a posicao da raiz e le o cabecalho do no interno
        fseek(f_index, config.raiz, SEEK_SET);
        TARVBMSDADOS raiz_dados;
        fread(&raiz_dados, sizeof(TARVBMSDADOS), 1, f_index);
        printf("A raiz aponta para folhas? %d (1 = Sim)\n", raiz_dados.filho_folha);
        
        char nome_folha[30];
        fread(nome_folha, sizeof(char) * 30, 1, f_index);
        printf("Nome do arquivo apontado: %s\n", nome_folha);
        
        fclose(f_index);
    }
    
    TFOLHA * folha = TFOLHA_carrega("Keanu Reeves");
    if(folha){
        TFOLHA_imprime(folha);
        free(folha->entidades);
        free(folha);
    }
    else{
        printf("\nNão achou\n");
    }
   
    // Lendo a folha_0.bin para ver se o dado foi gravado
    FILE *f_folha = fopen("../dados/folha_0.bin", "rb");
    if (f_folha) {
        printf("\n--- Lendo folha_0.bin ---\n");
        TDADOSFOLHA dados_folha;
        fread(&dados_folha, sizeof(TDADOSFOLHA), 1, f_folha);
        printf("Numero de chaves na folha: %d\n", dados_folha.nchaves);
        
        // Loop para ler todas as entidades gravadas na folha
        for(int i = 0; i < dados_folha.nchaves; i++) {
            TD lido;
            fread(&lido, sizeof(TD), 1, f_folha);
            printf("Entidade [%d]: %s (Ano: %d)\n", i, lido.dados.pessoa.nome, lido.dados.pessoa.ano);
        }
        
        fclose(f_folha);
    } else {
        printf("Arquivo folha_0.bin nao encontrado! main!\n");
    }

    return 0;
}