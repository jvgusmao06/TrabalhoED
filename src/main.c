#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/TARVBMS.h"
#include "header/TD.h"

// Função auxiliar para poupar linhas na hora de inserir
void insere_pessoa(char *nome, int ano) {
    TD ent;
    ent.tipo = TIPO_PESSOA;
    strcpy(ent.dados.pessoa.nome, nome);
    ent.dados.pessoa.ano = ano;
    TARVBMS_insere_um(&ent);
}

int main() 
{
    // Usamos t=2 para que o limite máximo seja 3 e o mínimo seja 1.
    // Assim forçamos a árvore a dividir e fundir muito mais rapidamente!
    int t = 2; 
    
    printf("======================================================\n");
    printf("1. INICIALIZANDO A ARVORE B+ (Ordem t = %d)\n", t);
    printf("======================================================\n");
    TARVBMS_inicializa(t);

    printf("\n2. Inserindo elementos...\n");
    insere_pessoa("Alien", 1979);
    insere_pessoa("Batman", 1989);
    insere_pessoa("Coringa", 2008);
    insere_pessoa("Dracula", 1931);
    insere_pessoa("Elektra", 1990); 
    insere_pessoa("Frodo", 2001);
    insere_pessoa("Gandalf", 2001);
    insere_pessoa("Hulk", 2003);

    printf("\n--- ARVORE APOS INSERCOES ---\n");
    TARVBMS_imprime();
    printf("\n");
    TARVBMS_imprime_folhas();

    printf("\n======================================================\n");
    printf("3. TESTANDO REMOCOES E BALANCEAMENTO\n");
    printf("======================================================\n");

    // CASO 1: A folha tem chaves suficientes, é só apagar e deslocar o vetor
    printf("\n-> CASO 1: Remocao simples (Sem Underflow)\n");
    printf("Retirando 'Hulk'...\n");
    TARVBMS_retira("Hulk");
    printf("\nArvore apos retirar Hulk:\n");
    TARVBMS_imprime();

    // CASO 2: A folha fica com 0 chaves (Underflow) mas o vizinho tem para emprestar
    printf("\n-> CASO 2: Remocao causando EMPRESTIMO (Redistribuicao)\n");
    printf("Retirando 'Gandalf'...\n");
    TARVBMS_retira("Gandalf");
    printf("\nArvore apos retirar Gandalf:\n");
    TARVBMS_imprime();

    // CASO 3: A folha fica com 0 chaves e o vizinho só tem 1 chave (limite mínimo).
    // Vai ter de ocorrer uma FUSÃO (Merge) de folhas!
    printf("\n-> CASO 3: Remocao causando FUSAO (Merge) e reajuste no Pai\n");
    printf("Retirando 'Frodo'...\n");
    TARVBMS_retira("Frodo");
    printf("\nArvore apos retirar Frodo:\n");
    TARVBMS_imprime();

    printf("\n======================================================\n");
    printf("4. CONFERINDO A LISTA DE FOLHAS FINAL\n");
    printf("======================================================\n");
    // Aqui verificamos se o encadeamento 'prox' sobreviveu às fusões!
    TARVBMS_imprime_folhas();

    return 0;
}