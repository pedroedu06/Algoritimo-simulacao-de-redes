#ifndef ROUTERS_H
#define ROUTERS_H

/*
 * Este arquivo define os tipos de dados da simulacao e as funcoes que o
 * main.c pode usar. A implementacao das funcoes fica no routers.c.
 *
 * A simulacao imita o roteamento por estado de enlace (usado pelo OSPF):
 * o roteador conhece o mapa completo da rede e calcula as suas rotas com
 * o algoritmo de Dijkstra.
 */

#include <stdbool.h>   /* permite usar o tipo bool (true/false) */
#include <limits.h>    /* traz o INT_MAX */

#define MAX_ROUTERS 6    /* a rede tem 6 roteadores, de R0 a R5 */
#define INF INT_MAX      /* representa o custo "infinito" (ainda sem caminho) */
#define SEM_ENLACE 0     /* na matriz de custos, 0 quer dizer "sem cabo" */

/*
 * Enlace representa uma ligacao entre dois roteadores em UM sentido:
 * sai de "origin", chega em "destiny" e custa "cost".
 */
typedef struct {
    int origin;
    int destiny;
    int cost;
} Enlace;

/*
 * Rede guarda o mapa da rede de duas formas:
 * - a lista "enlaces", com todas as ligacoes (igual ao Bellman-Ford);
 * - a matriz "custo", em que custo[a][b] e o custo do cabo entre a e b.
 *   O Dijkstra usa a matriz para descobrir rapido quem e vizinho de quem.
 */
typedef struct {
    int qtyRouters;                         /* quantidade de roteadores */
    int qtyEnlaces;                         /* enlaces ja gravados na lista */
    int maxEnlaces;                         /* tamanho maximo da lista */
    Enlace* enlaces;
    int custo[MAX_ROUTERS][MAX_ROUTERS];
} Rede;

/* Cria a rede vazia, com espaco para "maxEnlaces" enlaces. */
Rede* createRede(int V, int maxEnlaces);

/* Liga os roteadores u e v com um cabo de custo "cost" (ida e volta). */
void  addEnlace(Rede* rede, int u, int v, int cost);

/* Devolve a memoria usada pela rede. */
void  freeRede(Rede* rede);

/*
 * Calcula o menor custo do roteador "origin" ate todos os outros.
 * No final, dist[v] tem o menor custo ate v e previous[v] tem o
 * roteador que vem antes de v nesse caminho. Mostra o resultado de cada
 * passagem do laco principal.
 */
void dijkstra(Rede* rede, int origin, int dist[], int previous[]);

/* Mostra o caminho completo da origem ate o roteador v. */
void printPath(int previous[], int v, int origin);

/* Mostra os vizinhos de cada roteador e a matriz de custos iniciais. */
void printTopologia(Rede* rede);

/* Mostra a tabela final de um roteador (custo, anterior e rota). */
void printTabelaEncaminhamento(Rede* rede, int origin, int dist[], int previous[]);

#endif
