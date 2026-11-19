/*
 * Implementacao do algoritmo de Bellman-Ford e das funcoes que mostram o
 * resultado de cada fase.
 *
 * Como funciona, em poucas palavras: no comeco so sabemos que a origem
 * custa 0 e o resto custa "infinito". Depois passamos por todos os enlaces
 * perguntando "da para chegar no destino deste enlace mais barato passando
 * pela origem dele?". Se der, anotamos o novo custo. Repetimos essa
 * passada no maximo V-1 vezes, porque um caminho sem repeticao tem no
 * maximo V-1 enlaces. Se uma passada inteira nao mudar nada, ja terminou.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "routers.h"

/* Reserva memoria para a rede e para a lista de enlaces.
 * Se faltar memoria, avisa e encerra o programa. */
Rede *createRede(int V, int E)
{
    Rede *rede = (Rede *)malloc(sizeof(Rede));
    if (rede == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente\n");
        exit(1);
    }
    rede->qtyRouters = V;
    rede->qtyEnlaces = E;
    rede->enlaces = (Enlace *)malloc(E * sizeof(Enlace));
    if (rede->enlaces == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente\n");
        free(rede);
        exit(1);
    }
    return rede;
}

/* Imprime uma linha da tabela de passagens: o numero da passagem e, para
 * cada roteador, o custo atual e o roteador anterior (ex.: 3/R2). */
static void printLinhaPassagem(int passagem, int V, int dist[], int previous[])
{
    int v;
    char cel[16];

    printf("  %-8d", passagem);
    for (v = 0; v < V; v++) {
        if (dist[v] == INT_MAX)
            snprintf(cel, sizeof(cel), "INF/-");
        else
            snprintf(cel, sizeof(cel), "%d/R%d", dist[v], previous[v]);
        printf("| %-7s", cel);
    }
    printf("\n");
}

bool bellmanFord(Rede *rede, int origin, int dist[], int previous[])
{
    int V = rede->qtyRouters;
    int E = rede->qtyEnlaces;
    int i, j, round;

    /* Primeiro passo: todos os roteadores comecam com custo "infinito"
     * (INT_MAX) e sem roteador anterior (-1). So a origem comeca com 0. */
    for (i = 0; i < V; i++)
    {
        dist[i] = INT_MAX;
        previous[i] = -1;
    }
    dist[origin] = 0;
    previous[origin] = origin;

    /* Cabecalho da tabela com o resultado de cada passagem. */
    printf("RESULTADO DE CADA PASSAGEM (custo/roteador anterior):\n");
    printf("  %-8s", "Rodada");
    for (i = 0; i < V; i++)
        printf("| R%-6d", i);
    printf("\n");

    /* Segundo passo: fazemos ate V-1 rodadas. Em cada rodada olhamos
     * todos os enlaces, um por um. */
    for (round = 1; round <= V - 1; round++)
    {
        bool have_update = false;   /* marca se algo mudou nesta rodada */

        for (j = 0; j < E; j++)
        {
            int u = rede->enlaces[j].origin;
            int v = rede->enlaces[j].destiny;
            int weight = rede->enlaces[j].cost;

            /* Se ja sabemos chegar em u, e ir ate u e depois usar este
             * enlace sai mais barato do que o custo que v tem hoje,
             * trocamos o custo de v e anotamos que v agora vem de u.
             * Isso se chama "relaxar" o enlace. O teste de INT_MAX evita
             * somar em cima do infinito, o que estouraria o int. */
            if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
            {
                dist[v] = dist[u] + weight;
                previous[v] = u;
                have_update = true;
            }
        }

        /* Mostra como ficaram os custos no fim desta rodada. */
        printLinhaPassagem(round, V, dist, previous);

        /* Se uma rodada inteira passou sem mudar nenhum custo, as rotas ja
         * estao certas e as proximas rodadas tambem nao mudariam nada. */
        if (!have_update)
        {
            break;
        }
    }
    printf("\n");

    /* Terceiro passo: conferimos todos os enlaces mais uma vez. Se ainda
     * der para baixar algum custo, a rede tem um ciclo de custo negativo
     * (dando voltas nele o custo cairia para sempre) e nao existe resposta. */
    for (j = 0; j < E; j++)
    {
        int u = rede->enlaces[j].origin;
        int v = rede->enlaces[j].destiny;
        int weight = rede->enlaces[j].cost;

        if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
        {
            return false;
        }
    }

    return true;
}

/* Mostra o caminho ate v. Como so guardamos o roteador anterior de cada
 * um, a funcao chama a si mesma para mostrar primeiro o caminho ate o
 * anterior de v, e so depois escreve o proprio v. */
void printPath(int previous[], int v, int origin)
{
    if (v == origin) {
        printf("R%d", origin);
        return;
    }
    if (previous[v] == -1) {   /* nao existe caminho ate v */
        printf("-");
        return;
    }
    printPath(previous, previous[v], origin);
    printf(" -> R%d", v);
}

/* Imprime a linha de titulo das matrizes: R0 R1 R2 ... */
static void printCabecalhoMatriz(int V)
{
    int v;
    printf("%-6s", "");
    for (v = 0; v < V; v++)
        printf("%6s%d", "R", v);
    printf("\n");
}

/* Mostra os vizinhos de cada roteador e a matriz de custos
 * iniciais. A matriz e montada a partir da lista de enlaces: a posicao
 * [a][b] recebe o custo do cabo entre a e b, ou 0 se nao houver cabo. */
void printTopologia(Rede *rede)
{
    int V = rede->qtyRouters;
    int custo[MAX_ROUTERS][MAX_ROUTERS];
    int u, v, j;

    for (u = 0; u < V; u++)
        for (v = 0; v < V; v++)
            custo[u][v] = 0;
    for (j = 0; j < rede->qtyEnlaces; j++)
        custo[rede->enlaces[j].origin][rede->enlaces[j].destiny] = rede->enlaces[j].cost;

    printf("VIZINHOS DE CADA ROTEADOR (rotas de entrada/saida):\n");
    for (u = 0; u < V; u++) {
        int grau = 0;   /* quantos cabos este roteador tem */
        printf("  R%d: ", u);
        for (v = 0; v < V; v++)
            if (custo[u][v] != 0) {
                printf("%sR%d(%d)", grau ? ", " : "", v, custo[u][v]);
                grau++;
            }
        printf("   -> %d enlaces\n", grau);
    }
    printf("\n");

    printf("MATRIZ DE CUSTOS INICIAIS ('-' = sem enlace direto):\n");
    printCabecalhoMatriz(V);
    for (u = 0; u < V; u++) {
        printf("R%-5d", u);
        for (v = 0; v < V; v++) {
            if (u == v)
                printf("%7d", 0);
            else if (custo[u][v] == 0)
                printf("%7s", "-");
            else
                printf("%7d", custo[u][v]);
        }
        printf("\n");
    }
    printf("\n");
}

/* Tabela final de um roteador, com o menor custo, o roteador
 * anterior e o caminho completo ate cada destino. */
void printTabelaEncaminhamento(Rede *rede, int origin, int dist[], int previous[])
{
    int i;

    printf("TABELA DE ENCAMINHAMENTO FINAL:\n");
    printf("---------------------------------------------------------------\n");
    printf("%-10s | %-12s | %-12s | %-22s\n", "Destino", "Custo Min.", "Predecessor", "Rota Completa");
    printf("---------------------------------------------------------------\n");
    for (i = 0; i < rede->qtyRouters; i++) {
        printf("R%-9d | ", i);
        if (dist[i] == INT_MAX) {
            printf("%-12s | %-12s | %-22s\n", "Inalcancavel", "-", "-");
        } else {
            printf("%-12d | R%-11d | ", dist[i], previous[i]);
            printPath(previous, i, origin);
            printf("\n");
        }
    }
    printf("---------------------------------------------------------------\n");
    printf("Status: Rotas convergidas com sucesso (sem ciclos negativos).\n\n");
}
