/*
 * Implementacao do algoritmo de Dijkstra e das funcoes que mostram a
 * topologia e a tabela final na tela.
 *
 * Como funciona, em poucas palavras: no comeco so sabemos que a origem
 * custa 0 e o resto custa "infinito". A cada passo escolhemos, entre os
 * roteadores que ainda nao foram fechados, o que tem o menor custo. O
 * custo dele passa a ser definitivo (ele e "fechado"). Depois olhamos os
 * vizinhos dele e vemos se passar por ele deixa algum vizinho mais barato.
 * Repetimos ate fechar todos os roteadores.
 *
 * Isso so funciona porque nenhum custo e negativo: se o roteador escolhido
 * e o mais barato de todos os abertos, nenhum outro caminho pode chegar
 * nele por menos, ja que qualquer desvio so soma custo.
 *
 * Esta versao procura o menor custo olhando um por um (busca linear). Para
 * poucos roteadores e o jeito mais simples e e rapido o suficiente.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "routers.h"

/* Reserva memoria para a rede e deixa a matriz de custos zerada, ou seja,
 * sem nenhum cabo. Se faltar memoria, avisa e encerra o programa. */
Rede *createRede(int V, int maxEnlaces)
{
    Rede *rede = (Rede *)malloc(sizeof(Rede));
    if (rede == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente\n");
        exit(1);
    }
    rede->qtyRouters = V;
    rede->qtyEnlaces = 0;
    rede->maxEnlaces = maxEnlaces;
    rede->enlaces = (Enlace *)malloc(maxEnlaces * sizeof(Enlace));
    if (rede->enlaces == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente\n");
        free(rede);
        exit(1);
    }
    memset(rede->custo, 0, sizeof(rede->custo));
    return rede;
}

/* Coloca um enlace (em um sentido so) no fim da lista. */
static void gravaEnlace(Rede *rede, int origin, int destiny, int cost)
{
    Enlace *e = &rede->enlaces[rede->qtyEnlaces++];
    e->origin = origin;
    e->destiny = destiny;
    e->cost = cost;
}

/* Liga u e v com um cabo. O cabo funciona nos dois sentidos, entao
 * gravamos a ida e a volta na lista e tambem nas duas posicoes da matriz. */
void addEnlace(Rede *rede, int u, int v, int cost)
{
    if (rede->qtyEnlaces + 2 > rede->maxEnlaces) {
        fprintf(stderr, "Erro: capacidade de enlaces excedida\n");
        exit(1);
    }
    gravaEnlace(rede, u, v, cost);
    gravaEnlace(rede, v, u, cost);
    rede->custo[u][v] = cost;
    rede->custo[v][u] = cost;
}

/* Devolve a memoria da lista de enlaces e da propria rede. */
void freeRede(Rede *rede)
{
    free(rede->enlaces);
    free(rede);
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
 * iniciais (so os cabos diretos). */
void printTopologia(Rede *rede)
{
    int V = rede->qtyRouters;
    int u, v;

    printf("VIZINHOS DE CADA ROTEADOR (rotas de entrada/saida):\n");
    for (u = 0; u < V; u++) {
        int grau = 0;   /* quantos cabos este roteador tem */
        printf("  R%d: ", u);
        for (v = 0; v < V; v++)
            if (rede->custo[u][v] != SEM_ENLACE) {
                printf("%sR%d(%d)", grau ? ", " : "", v, rede->custo[u][v]);
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
            else if (rede->custo[u][v] == SEM_ENLACE)
                printf("%7s", "-");
            else
                printf("%7d", rede->custo[u][v]);
        }
        printf("\n");
    }
    printf("\n");
}

/* Imprime uma linha da tabela de passagens: o numero da passagem, o
 * roteador fechado nela e, para cada roteador, o custo atual e o roteador
 * anterior (ex.: 3/R2). */
static void printLinhaPassagem(int passagem, int fechou, int V, int dist[], int previous[])
{
    int v;
    char cel[16];

    printf("  %-9d| R%-6d", passagem, fechou);
    for (v = 0; v < V; v++) {
        if (dist[v] == INF)
            snprintf(cel, sizeof(cel), "INF/-");
        else
            snprintf(cel, sizeof(cel), "%d/R%d", dist[v], previous[v]);
        printf("| %-7s", cel);
    }
    printf("\n");
}

void dijkstra(Rede *rede, int origin, int dist[], int previous[])
{
    int V = rede->qtyRouters;
    bool fechado[MAX_ROUTERS];   /* fechado[v] e true quando o custo de v ja e definitivo */
    int passo, u, v, w;

    /* Primeiro passo: todos comecam com custo infinito, sem roteador
     * anterior (-1) e abertos. So a origem comeca com custo 0. */
    for (v = 0; v < V; v++) {
        dist[v] = INF;
        previous[v] = -1;
        fechado[v] = false;
    }
    dist[origin] = 0;
    previous[origin] = origin;

    /* Cabecalho da tabela com o resultado de cada passagem. */
    printf("RESULTADO DE CADA PASSAGEM (custo/roteador anterior):\n");
    printf("  %-9s| %-7s", "Passagem", "Fechou");
    for (v = 0; v < V; v++)
        printf("| R%-6d", v);
    printf("\n");

    /* Cada volta deste laco fecha exatamente um roteador, entao sao no
     * maximo V voltas. */
    for (passo = 1; passo <= V; passo++) {

        /* Procura, entre os roteadores abertos, o de menor custo. Em caso
         * de empate fica o de menor numero. */
        u = -1;
        for (v = 0; v < V; v++) {
            if (!fechado[v] && dist[v] != INF && (u == -1 || dist[v] < dist[u]))
                u = v;
        }

        /* Se nao achou ninguem, os roteadores que sobraram nao tem caminho
         * a partir da origem, e o algoritmo termina. */
        if (u == -1)
            break;

        /* Fecha o roteador escolhido: o custo dele nao muda mais. */
        fechado[u] = true;

        /* Olha cada vizinho v do roteador escolhido. Se v ainda esta
         * aberto e chegar nele passando por u sai mais barato que o custo
         * atual de v, troca o custo e anota que v agora vem de u.
         * Isso se chama "relaxar" o enlace. */
        for (v = 0; v < V; v++) {
            w = rede->custo[u][v];
            if (w == SEM_ENLACE || fechado[v])
                continue;   /* nao e vizinho, ou ja esta fechado */
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                previous[v] = u;
            }
        }

        /* Mostra como ficaram os custos no fim desta passagem. */
        printLinhaPassagem(passo, u, V, dist, previous);
    }
    printf("\n");
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

/* Tabela final de um roteador, com o menor custo, o roteador
 * anterior e o caminho completo ate cada destino. E o mesmo formato
 * usado no Bellman-Ford. */
void printTabelaEncaminhamento(Rede *rede, int origin, int dist[], int previous[])
{
    int i;

    printf("TABELA DE ENCAMINHAMENTO FINAL:\n");
    printf("---------------------------------------------------------------\n");
    printf("%-10s | %-12s | %-12s | %-22s\n", "Destino", "Custo Min.", "Predecessor", "Rota Completa");
    printf("---------------------------------------------------------------\n");
    for (i = 0; i < rede->qtyRouters; i++) {
        printf("R%-9d | ", i);
        if (dist[i] == INF) {
            printf("%-12s | %-12s | %-22s\n", "Inalcancavel", "-", "-");
        } else {
            printf("%-12d | R%-11d | ", dist[i], previous[i]);
            printPath(previous, i, origin);
            printf("\n");
        }
    }
    printf("---------------------------------------------------------------\n");
    printf("Status: Arvore de caminhos minimos calculada (todos os custos >= 0).\n\n");
}
