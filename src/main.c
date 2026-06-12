#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/TARVBMS.h"
#include "header/util.h"
#include "header/TFOLHA.h"

int main()
{
    int t = 10;

    /* Limpa dados antigos */
    system("rm -f ../dados/*.bin");

    /* Inicializa a árvore B+ */
    TARVBMS_inicializa(t);

    /* ========== INSERÇÃO ========== */
    printf("========================================\n");
    printf("  PREENCHENDO ARVORE COM Nodes.txt\n");
    printf("========================================\n\n");
    TARVBMS_preenche_arvore("../entrada/Nodes.txt");

    /* ========== ESTRUTURA DA ÁRVORE ========== */
    printf("\n========================================\n");
    printf("  ESTRUTURA DA ARVORE B+\n");
    printf("========================================\n\n");
    TARVBMS_imprime();

    /* ========== FOLHAS ENCADEADAS ========== */
    printf("\n========================================\n");
    printf("  FOLHAS ENCADEADAS (ordem alfabetica)\n");
    printf("========================================\n\n");
    TARVBMS_imprime_folhas();

    /* ========== BUSCA ========== */
    printf("\n========================================\n");
    printf("  TESTE DE BUSCA\n");
    printf("========================================\n\n");

    char *buscas[] = {
        "Keanu Reeves",
        "The Matrix",
        "Tom Hanks",
        "Forrest Gump",
        "Nao Existe Este Nome",
        "Hugo Weaving"
    };
    int n_buscas = sizeof(buscas) / sizeof(buscas[0]);

    for (int i = 0; i < n_buscas; i++)
    {
        TFOLHA *resultado = TFOLHA_carrega(buscas[i]);
        if (resultado)
        {
            printf("  [OK] '%s' encontrado.\n", buscas[i]);
            free(resultado->entidades);
            free(resultado);
        }
        else
        {
            printf("  [--] '%s' NAO encontrado.\n", buscas[i]);
        }
    }

    /* ========== REMOÇÃO ========== */
    printf("\n========================================\n");
    printf("  TESTE DE REMOCAO\n");
    printf("========================================\n\n");

    char *remocoes[] = {
        "Keanu Reeves",
        "The Matrix",
        "Tom Hanks"
    };
    int n_remocoes = sizeof(remocoes) / sizeof(remocoes[0]);

    for (int i = 0; i < n_remocoes; i++)
    {
        printf("Removendo '%s'...\n", remocoes[i]);
        TARVBMS_retira(remocoes[i]);
    }

    /* ========== VERIFICAÇÃO PÓS-REMOÇÃO ========== */
    printf("\n========================================\n");
    printf("  VERIFICACAO POS-REMOCAO\n");
    printf("========================================\n\n");

    for (int i = 0; i < n_remocoes; i++)
    {
        TFOLHA *resultado = TFOLHA_carrega(remocoes[i]);
        if (resultado)
        {
            printf("  [ERRO] '%s' ainda encontrado apos remocao!\n", remocoes[i]);
            free(resultado->entidades);
            free(resultado);
        }
        else
        {
            printf("  [OK]   '%s' removido com sucesso.\n", remocoes[i]);
        }
    }

    /* Verifica que outros ainda existem */
    char *verificar[] = {"Hugo Weaving", "Forrest Gump"};
    for (int i = 0; i < 2; i++)
    {
        TFOLHA *resultado = TFOLHA_carrega(verificar[i]);
        if (resultado)
        {
            printf("  [OK]   '%s' ainda presente na arvore.\n", verificar[i]);
            free(resultado->entidades);
            free(resultado);
        }
        else
        {
            printf("  [ERRO] '%s' desapareceu da arvore!\n", verificar[i]);
        }
    }

    /* ========== ESTADO FINAL ========== */
    printf("\n========================================\n");
    printf("  FOLHAS APOS REMOCOES\n");
    printf("========================================\n\n");
    TARVBMS_imprime_folhas();

    printf("\n========================================\n");
    printf("  TESTE CONCLUIDO\n");
    printf("========================================\n");

    return 0;
}