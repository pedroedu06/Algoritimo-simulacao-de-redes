#ifndef ROUTERS_H
#define ROUTERS_H

/*
 * Este arquivo define os tipos de dados da simulacao e as funcoes que o
 * main.c pode usar. A implementacao das funcoes fica no routers.c.
 */

#include <stdbool.h>   /* permite usar o tipo bool (true/false) */

#define MAX_ROUTERS 10   /* limite de roteadores usado nas tabelas de impressao */

/*
 * Enlace representa uma ligacao entre dois roteadores em UM sentido:
 * sai de "origin", chega em "destiny" e custa "cost".
 * Um cabo de verdade funciona nos dois sentidos, por isso cada cabo
 * vira dois enlaces (ida e volta).
 */
typedef struct {
    int origin;
    int destiny;
    int cost;
} Enlace;

/*
 * Rede guarda a quantidade de roteadores, a quantidade de enlaces e a
 * lista com todos os enlaces. O Bellman-Ford so precisa dessa lista.
 */
typedef struct {
    int qtyRouters;
    int qtyEnlaces;
    Enlace* enlaces;
} Rede;

/* Cria a rede reservando memoria para E enlaces. */
Rede* createRede(int V, int E);

/*
 * Calcula o menor custo do roteador "origin" ate todos os outros.
 * No final, dist[v] tem o menor custo ate v e previous[v] tem o roteador
 * que vem antes de v nesse caminho. Mostra o resultado de cada rodada.
 * Retorna false se a rede tiver um ciclo de custo negativo.
 */
bool bellmanFord(Rede* rede, int origin, int dist[], int previous[]);

/* Mostra o caminho completo da origem ate o roteador v. */
void printPath(int previous[], int v, int origin);

/* Mostra os vizinhos de cada roteador e a matriz de custos iniciais. */
void printTopologia(Rede* rede);

/* Mostra a tabela final de um roteador (custo, anterior e rota). */
void printTabelaEncaminhamento(Rede* rede, int origin, int dist[], int previous[]);

#endif
