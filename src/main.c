#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/TARVBMS.h"
#include "header/util.h"

int main() {
    int t = 2;

    TARVBMS_inicializa(t);

    TARVBMS_preenche_arvore("../entrada/Nodes.txt");

    TARVBMS_imprime_folhas();
}