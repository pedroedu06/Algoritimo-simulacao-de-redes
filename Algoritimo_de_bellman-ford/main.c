/*
 * Programa principal da simulacao com Bellman-Ford.
 * A rede tem 5 roteadores (R0 a R4) ligados por 8 cabos. O programa
 * mostra a topologia com os custos iniciais, o resultado de cada rodada
 * do algoritmo a partir de R0 e a tabela de encaminhamento final.
 *
 * Para compilar: gcc -Wall -o routers.exe main.c routers.c
 * Para executar: routers.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include "routers.h"

#define QTD_ROUTERS 5                 /* quantidade de roteadores */
#define QTD_LINKS   8                 /* quantidade de cabos */
#define QTD_ENLACES (2 * QTD_LINKS)   /* cada cabo vira ida e volta */

int main(void)
{
    int V = QTD_ROUTERS;
    int E = QTD_ENLACES;
    int dist[QTD_ROUTERS];       /* menor custo de R0 ate cada roteador */
    int previous[QTD_ROUTERS];   /* roteador anterior no melhor caminho */
    int sourceRouter = 0;        /* as rotas sao calculadas a partir de R0 */
    int i, idx;
    Rede *rede;

    /* A topologia: cada linha e um cabo no formato
     * {roteador A, roteador B, custo}. */
    int linksConfig[QTD_LINKS][3] = {
        {0, 1, 4}, /* R0 <-> R1 custo 4 */
        {0, 2, 2}, /* R0 <-> R2 custo 2 */
        {0, 4, 8}, /* R0 <-> R4 custo 8 */
        {1, 2, 1}, /* R1 <-> R2 custo 1 */
        {1, 3, 7}, /* R1 <-> R3 custo 7 */
        {2, 3, 3}, /* R2 <-> R3 custo 3 */
        {2, 4, 5}, /* R2 <-> R4 custo 5 */
        {3, 4, 1}  /* R3 <-> R4 custo 1 */
    };

    rede = createRede(V, E);

    /* Cada cabo e gravado duas vezes na lista de enlaces: uma vez de A
     * para B (ida) e outra de B para A (volta), com o mesmo custo. */
    idx = 0;
    for (i = 0; i < QTD_LINKS; i++) {
        int u = linksConfig[i][0];
        int v = linksConfig[i][1];
        int cost = linksConfig[i][2];

        rede->enlaces[idx].origin  = u;
        rede->enlaces[idx].destiny = v;
        rede->enlaces[idx].cost    = cost;
        idx++;

        rede->enlaces[idx].origin  = v;
        rede->enlaces[idx].destiny = u;
        rede->enlaces[idx].cost    = cost;
        idx++;
    }

    printf("===============================================================\n");
    printf("  ROTEAMENTO COM BELLMAN-FORD (VETOR DE DISTANCIAS - ORIGEM: R%d)\n", sourceRouter);
    printf("===============================================================\n\n");

    /* Primeiro mostra a topologia com os custos iniciais. */
    printTopologia(rede);

    /* Roda o algoritmo, que mostra o resultado de cada rodada. Se deu
     * certo, mostra a tabela final com o custo, o roteador anterior e o
     * caminho completo ate cada destino. */
    if (bellmanFord(rede, sourceRouter, dist, previous))
        printTabelaEncaminhamento(rede, sourceRouter, dist, previous);
    else
        printf("Alerta: Ciclo de peso negativo detectado na topologia!\n");

    /* Devolve a memoria que foi reservada. */
    free(rede->enlaces);
    free(rede);
    return 0;
}
