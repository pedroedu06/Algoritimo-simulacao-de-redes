/*
 * Programa principal da simulacao com Dijkstra.
 * A rede tem 6 roteadores (R0 a R5) ligados por 9 cabos. O programa
 * mostra a topologia com os custos iniciais, o resultado de cada passagem
 * do algoritmo a partir de R0 e a tabela de encaminhamento final.
 *
 * Para compilar: gcc -Wall -o routers.exe main.c routers.c
 * Para executar: routers.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include "routers.h"

#define QTD_LINKS 9   /* quantidade de cabos na rede */

int main(void)
{
    int V = MAX_ROUTERS;
    int dist[MAX_ROUTERS];       /* menor custo de R0 ate cada roteador */
    int previous[MAX_ROUTERS];   /* roteador anterior no melhor caminho */
    int sourceRouter = 0;        /* as rotas sao calculadas a partir de R0 */
    int i;
    Rede *rede;

    /* A topologia: cada linha e um cabo no formato
     * {roteador A, roteador B, custo}. Todo roteador tem pelo menos
     * dois cabos (rotas de entrada e saida). */
    int linksConfig[QTD_LINKS][3] = {
        {0, 1, 4},   /* R0 <-> R1 custo 4 */
        {0, 2, 2},   /* R0 <-> R2 custo 2 */
        {1, 2, 1},   /* R1 <-> R2 custo 1 */
        {1, 3, 5},   /* R1 <-> R3 custo 5 */
        {2, 3, 8},   /* R2 <-> R3 custo 8 */
        {2, 4, 10},  /* R2 <-> R4 custo 10 */
        {3, 4, 2},   /* R3 <-> R4 custo 2 */
        {3, 5, 6},   /* R3 <-> R5 custo 6 */
        {4, 5, 3}    /* R4 <-> R5 custo 3 */
    };

    /* Cria a rede com espaco para ida e volta de cada cabo e liga os
     * roteadores conforme a tabela acima. */
    rede = createRede(V, 2 * QTD_LINKS);
    for (i = 0; i < QTD_LINKS; i++)
        addEnlace(rede, linksConfig[i][0], linksConfig[i][1], linksConfig[i][2]);

    printf("===============================================================\n");
    printf("  ROTEAMENTO COM DIJKSTRA (ESTADO DE ENLACE - ORIGEM: R%d)\n", sourceRouter);
    printf("===============================================================\n\n");

    /* No OSPF cada roteador avisa a rede inteira quais cabos tem, e todos
     * acabam com o mesmo mapa. Primeiro mostramos esse mapa, com os
     * custos iniciais. */
    printTopologia(rede);

    /* R0 roda o Dijkstra, que mostra o resultado de cada passagem, e
     * depois mostramos a tabela final. */
    dijkstra(rede, sourceRouter, dist, previous);
    printTabelaEncaminhamento(rede, sourceRouter, dist, previous);

    freeRede(rede);
    return 0;
}
