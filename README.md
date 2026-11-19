# Algoritmos de roteamento: Bellman-Ford e Dijkstra em C

Este repositório simula como roteadores escolhem o caminho mais barato até cada destino, usando os dois algoritmos clássicos de caminho mínimo:

| Parte | Algoritmo | Pasta | Usado em redes no |
|---|---|---|---|
| [1](#parte-1--algoritmo-de-bellman-ford) | Bellman-Ford | [Algoritimo_de_bellman-ford](Algoritimo_de_bellman-ford) | roteamento por **vetor de distâncias** (RIP) |
| [2](#parte-2--algoritmo-de-dijkstra) | Dijkstra | [Algoritimo_de_dijkstra](Algoritimo_de_dijkstra) | roteamento por **estado de enlace** (OSPF) |
| [3](#parte-3--comparação) | Comparação | | |
| [4](#parte-4--roteiro-para-a-apresentação) | Roteiro da apresentação | | |

### Como compilar e rodar

Os dois projetos têm a mesma estrutura (`main.c`, `routers.c`, `routers.h`) e compilam do mesmo jeito. Dentro da pasta de cada um:

```bash
gcc -Wall -o routers.exe main.c routers.c
```
```bash
routers.exe
```

Os códigos foram testados em C90, C99, C11, C17, C23 e também como C++, então funcionam em compiladores antigos (como o Dev-C++) e novos. Os `routers.exe` já compilados rodam em qualquer Windows 10 ou 11 sem instalar nada.

### Ideias que valem para os dois algoritmos

- A rede vira um **grafo**: cada roteador é um vértice e cada cabo é uma aresta com um **custo** (pode representar atraso, distância ou o inverso da velocidade do link).
- O objetivo é achar, a partir de um roteador de origem, o **caminho de menor custo** até cada outro roteador.
- Os dois guardam dois vetores: `dist[v]` (o menor custo conhecido até v) e `previous[v]` (o roteador que vem antes de v nesse caminho). Seguindo o `previous` de trás para frente, remonta-se a rota inteira.
- Os dois começam com `dist[origem] = 0` e todos os outros com custo "infinito".
- Os dois usam a mesma operação básica, o **relaxamento**: para um enlace u → v com custo w, se `dist[u] + w < dist[v]`, então passar por u é melhor, e trocamos `dist[v] = dist[u] + w` e `previous[v] = u`.

A diferença entre eles está em **quais enlaces relaxar e em que ordem**.

---

## Parte 1 — Algoritmo de Bellman-Ford

### 1.1 Pesquisa

**O problema.** Dado um roteador de origem, achar o caminho mais barato até todos os outros. Diferente do Dijkstra, o Bellman-Ford **aceita custos negativos**. Ele só não tem resposta quando existe um **ciclo de custo negativo**, porque aí seria possível dar voltas nele e baixar o custo para sempre.

**A base: subestrutura ótima.** As aulas do IME-USP partem de uma propriedade simples: qualquer trecho de um caminho mínimo também é um caminho mínimo. Então o melhor custo até um roteador é sempre o melhor custo até algum vizinho dele mais o custo do cabo entre os dois. O relaxamento é exatamente esse teste.

**Como funciona.** Feofiloff (IME-USP) descreve o algoritmo como uma sequência de **rodadas**. Em cada rodada todos os enlaces são examinados, e todo enlace "tenso" (aquele em que `dist[u] + custo < dist[v]`) é relaxado:

1. `dist[origem] = 0` e todos os outros `dist = ∞`.
2. Repete **V − 1 vezes** (V = número de roteadores): passa por **todos** os enlaces e relaxa cada um.
3. Passa por todos os enlaces **mais uma vez**. Se ainda der para melhorar algum custo, existe ciclo negativo.

**Por que V − 1 rodadas bastam.** Um caminho mínimo sem repetir roteador tem no máximo V − 1 enlaces. Depois da 1ª rodada, todos os destinos cujo melhor caminho tem 1 enlace já estão certos. Depois da 2ª, os de até 2 enlaces, e assim por diante. Na rodada V − 1 todos estão certos. Se uma rodada termina sem mudar nada, as próximas também não mudariam, então dá para parar antes (o código faz isso).

**Ciclo negativo.** Se depois das V − 1 rodadas algum enlace ainda consegue baixar um custo, é porque existe um caminho "melhor" com V ou mais enlaces, ou seja, que repete roteador. Isso só acontece se houver um ciclo negativo. Feofiloff mostra também como reencontrar o ciclo seguindo o vetor de pais.

**Complexidade.** São até V − 1 rodadas, cada uma passando pelos E enlaces: **O(V · E)**. É mais lento que o Dijkstra, e esse é o preço de aceitar custos negativos.

**Bellman-Ford nas redes (vetor de distâncias).** Os slides de redes de Princeton (COS 461), Washington (CSE 461) e KTH mostram que o roteamento por **vetor de distâncias**, usado pelo protocolo **RIP**, é uma versão *distribuída* do Bellman-Ford:

- Cada roteador só conhece os custos até os **vizinhos**, e não o mapa inteiro.
- De tempos em tempos, cada roteador envia aos vizinhos o seu vetor de distâncias ("chego em R3 com custo 5, em R4 com custo 6...").
- Ao receber, faz o relaxamento: `meu custo até X = menor de (custo até o vizinho + custo do vizinho até X)`.
- Com o tempo, todos os vetores **convergem** para os custos mínimos.
- Problema conhecido: a **contagem ao infinito**. Quando um link cai, dois roteadores podem ficar se enganando e aumentando o custo aos poucos. O RIP limita isso tratando 16 saltos como infinito e usando técnicas como *split horizon* e *poison reverse*.

**Observação sobre o nosso código.** O programa roda o Bellman-Ford de forma **centralizada**: ele tem a lista com todos os enlaces e faz as rodadas em um lugar só. O resultado final é o mesmo que os roteadores alcançariam trocando vetores, mas sem simular as mensagens.

**Fontes**
- Vídeo *Algoritmo de Bellman-Ford - Algoritmos em Grafos*: https://www.youtube.com/watch?v=dx0vUNkARSw
- Paulo Feofiloff, *Algoritmo de Bellman-Ford*, IME-USP: https://users.ime.usp.br/~pf/algoritmos_para_grafos/aulas/bellman-ford.html
- *Algoritmo de Bellman-Ford*, Algoritmos em Grafos (MAC0328), IME-USP, 2012: https://users.ime.usp.br/~am/328-12/aulas/aula-0426.pdf
- Humberto César Brandão de Oliveira, *Aula 12 – Caminho Mínimo: Algoritmo de Bellman-Ford*, Algoritmos em Grafos, UNIFAL: https://www.bcc.unifal-mg.edu.br/~humberto/disciplinas/2010_2_grafos/pdf_aulas/aula_12.pdf
- Aula 24, UFSCar: https://www.aloc.ufscar.br/felice/ensino/2025s2paa/aula24.pdf
- Princeton COS 461, *Distance-Vector Routing*: https://www.cs.princeton.edu/courses/archive/spring23/cos461/lectures/lec09-dvrouting.pdf
- University of Washington CSE 461, lecture 13: https://courses.cs.washington.edu/courses/cse461/08au/lectures/lect13.pdf
- KTH DD2490, *Routing / RIP*: https://www.csc.kth.se/utbildning/kth/kurser/DD2490/ipro1-10/lectures/routing_rip.pdf
- Cormen, Leiserson, Rivest e Stein, *Introduction to Algorithms* (CLRS), capítulo "Single-Source Shortest Paths".

### 1.2 Topologia usada

5 roteadores (R0 a R4) e 8 cabos. Todo roteador tem pelo menos 2 ligações.

```
              4                7
      R0 ------------ R1 ------------ R3
      | \            /               / |
      |  \ 2      1 /             3 /  |
      |   \        /               /   | 1
    8 |     \    /               /     |
      |       R2 --------------+       |
      |         \                      |
      |          \ 5                   |
      |           \                    |
      +---------- R4 ------------------+
```

| Roteador | Vizinhos (custo) | Nº de ligações |
|---|---|---|
| R0 | R1(4), R2(2), R4(8) | 3 |
| R1 | R0(4), R2(1), R3(7) | 3 |
| R2 | R0(2), R1(1), R3(3), R4(5) | 4 |
| R3 | R1(7), R2(3), R4(1) | 3 |
| R4 | R0(8), R2(5), R3(1) | 3 |

**Custos iniciais** (só os cabos diretos, `-` = sem cabo):

|    | R0 | R1 | R2 | R3 | R4 |
|----|----|----|----|----|----|
| **R0** | 0 | 4 | 2 | - | 8 |
| **R1** | 4 | 0 | 1 | 7 | - |
| **R2** | 2 | 1 | 0 | 3 | 5 |
| **R3** | - | 7 | 3 | 0 | 1 |
| **R4** | 8 | - | 5 | 1 | 0 |

**Custos finais** (menor custo entre cada par). A linha de R0 é o que o programa calcula. As outras linhas foram calculadas à parte, trocando a origem:

|    | R0 | R1 | R2 | R3 | R4 |
|----|----|----|----|----|----|
| **R0** | 0 | 3 | 2 | 5 | 6 |
| **R1** | 3 | 0 | 1 | 4 | 5 |
| **R2** | 2 | 1 | 0 | 3 | 4 |
| **R3** | 5 | 4 | 3 | 0 | 1 |
| **R4** | 6 | 5 | 4 | 1 | 0 |

Quatro cabos diretos perdem para um caminho indireto:

| Par | Custo direto | Custo final | Caminho mais barato |
|---|---|---|---|
| R0 ↔ R1 | 4 | **3** | R0 → R2 → R1 |
| R0 ↔ R4 | 8 | **6** | R0 → R2 → R3 → R4 |
| R1 ↔ R3 | 7 | **4** | R1 → R2 → R3 |
| R2 ↔ R4 | 5 | **4** | R2 → R3 → R4 |

### 1.3 Como o código foi feito

- **[routers.h](Algoritimo_de_bellman-ford/routers.h)**: define o `Enlace` (origem, destino, custo) e a `Rede` (quantidade de roteadores, quantidade de enlaces e a lista de enlaces).
- **[main.c](Algoritimo_de_bellman-ford/main.c)**: descreve a topologia numa tabela `linksConfig` (um cabo por linha) e grava cada cabo duas vezes na lista, uma para a ida e outra para a volta. Depois mostra a topologia, roda o algoritmo e imprime a tabela de encaminhamento.
- **[routers.c](Algoritimo_de_bellman-ford/routers.c)**:
  - `createRede` reserva a memória.
  - `bellmanFord` faz os três passos: inicializa, roda até V − 1 rodadas relaxando todos os enlaces (parando antes se uma rodada não mudar nada) e confere se ficou algum ciclo negativo. No fim de cada rodada, imprime uma linha com o custo e o roteador anterior de cada roteador.
  - `printPath` remonta a rota seguindo o `previous` de trás para frente.
  - `printTopologia` mostra os vizinhos de cada roteador e a matriz de custos iniciais.
  - `printTabelaEncaminhamento` mostra a tabela final de R0.

### 1.4 Passo a passo (origem R0)

A lista tem 16 enlaces (8 cabos × ida e volta), percorridos sempre na mesma ordem.

**Rodada 1:**

| Enlace | Conta | Resultado |
|---|---|---|
| R0 → R1 | 0 + 4 = 4 < ∞ | **R1 = 4** (vem de R0) |
| R0 → R2 | 0 + 2 = 2 < ∞ | **R2 = 2** (vem de R0) |
| R0 → R4 | 0 + 8 = 8 < ∞ | **R4 = 8** (vem de R0) |
| R2 → R1 | 2 + 1 = 3 < 4 | **R1 = 3** (vem de R2) |
| R1 → R3 | 3 + 7 = 10 < ∞ | **R3 = 10** (vem de R1) |
| R2 → R3 | 2 + 3 = 5 < 10 | **R3 = 5** (vem de R2) |
| R2 → R4 | 2 + 5 = 7 < 8 | **R4 = 7** (vem de R2) |
| R3 → R4 | 5 + 1 = 6 < 7 | **R4 = 6** (vem de R3) |
| demais 8 enlaces | não melhoram nada | |

No fim da rodada 1: R0 = 0, R1 = 3, R2 = 2, R3 = 5, R4 = 6.

**Rodada 2:** nenhum enlace consegue baixar nenhum custo, então o algoritmo **para antes** das 4 rodadas possíveis (V − 1 = 4).

**Verificação final:** nenhum enlace melhora mais nada, portanto **não há ciclo negativo**.

Nesta topologia tudo se resolveu na 1ª rodada porque a ordem dos enlaces na lista ajudou: quando o algoritmo chega em um enlace, o roteador de saída dele quase sempre já tem o custo certo. Com outra ordem seriam necessárias mais rodadas, mas o resultado final seria o mesmo.

### 1.5 Resultado final

```
===============================================================
  ROTEAMENTO COM BELLMAN-FORD (VETOR DE DISTANCIAS - ORIGEM: R0)
===============================================================

VIZINHOS DE CADA ROTEADOR (rotas de entrada/saida):
  R0: R1(4), R2(2), R4(8)   -> 3 enlaces
  R1: R0(4), R2(1), R3(7)   -> 3 enlaces
  R2: R0(2), R1(1), R3(3), R4(5)   -> 4 enlaces
  R3: R1(7), R2(3), R4(1)   -> 3 enlaces
  R4: R0(8), R2(5), R3(1)   -> 3 enlaces

MATRIZ DE CUSTOS INICIAIS ('-' = sem enlace direto):
           R0     R1     R2     R3     R4
R0          0      4      2      -      8
R1          4      0      1      7      -
R2          2      1      0      3      5
R3          -      7      3      0      1
R4          8      -      5      1      0

RESULTADO DE CADA PASSAGEM (custo/roteador anterior):
  Rodada  | R0     | R1     | R2     | R3     | R4
  1       | 0/R0   | 3/R2   | 2/R0   | 5/R2   | 6/R3
  2       | 0/R0   | 3/R2   | 2/R0   | 5/R2   | 6/R3

TABELA DE ENCAMINHAMENTO FINAL:
---------------------------------------------------------------
Destino    | Custo Min.   | Predecessor  | Rota Completa
---------------------------------------------------------------
R0         | 0            | R0           | R0
R1         | 3            | R2           | R0 -> R2 -> R1
R2         | 2            | R0           | R0 -> R2
R3         | 5            | R2           | R0 -> R2 -> R3
R4         | 6            | R3           | R0 -> R2 -> R3 -> R4
---------------------------------------------------------------
Status: Rotas convergidas com sucesso (sem ciclos negativos).
```

R0 manda todo o tráfego pelo cabo até R2. Os cabos diretos R0–R1 (custo 4) e R0–R4 (custo 8) acabam não sendo usados.

---

## Parte 2 — Algoritmo de Dijkstra

### 2.1 Pesquisa

**O problema.** É o mesmo do Bellman-Ford: o menor caminho de uma origem até todos os outros. A diferença é que o Dijkstra **exige custos não negativos**. Em troca, é bem mais rápido.

**Como funciona.** A formulação da Unicamp (baseada no CLRS) e a do Feofiloff são equivalentes:

1. `dist[origem] = 0` e todos os outros `dist = ∞`. Ninguém está "fechado".
2. Escolhe, entre os roteadores ainda **abertos**, o de **menor custo** e **fecha** esse roteador: o custo dele vira definitivo.
3. Relaxa os enlaces que saem dele, ou seja, vê se passar por ele deixa algum vizinho aberto mais barato.
4. Repete até fechar todos (ou até sobrarem só roteadores inalcançáveis).

Cada fonte usa um nome para o conjunto dos fechados: **S** no CLRS, vértices **maduros** no Feofiloff e **N'** no livro de redes de Kurose & Ross. O freeCodeCamp fala simplesmente em nós **visitados**.

**Por que o custo do fechado é definitivo.** O roteador escolhido é o mais barato de todos os abertos. Qualquer outro caminho até ele teria que passar por algum roteador aberto, que já custa pelo menos o mesmo. Como nenhum custo é negativo, continuar a partir dali só soma, então ninguém chega nele por menos.

**Por que não aceita custos negativos.** Com um custo negativo, um caminho descoberto depois poderia baixar o custo de um roteador **já fechado**. O Dijkstra nunca volta a olhar um fechado, então a resposta sairia errada. Nesses casos usa-se o Bellman-Ford. Em redes isso não é problema, porque custo de link é sempre positivo.

**Complexidade.**

| Como achar o menor aberto | Tempo | Quando compensa |
|---|---|---|
| Procurar um por um (a usada aqui) | O(V²) | redes pequenas ou densas |
| Fila de prioridade (heap) | O((V + E) log V) | redes grandes e esparsas |

Feofiloff também alerta para o estouro de `INT_MAX + custo`. Por isso o código só soma quando o custo não é infinito.

**Dijkstra nas redes (estado de enlace).** Os slides de Princeton (COS 461), Michigan (EECS 489), Washington (CSE 461) e Duke (ECE 356, OSPF) descrevem o roteamento por **estado de enlace**, usado pelo protocolo **OSPF**:

1. Cada roteador descobre os vizinhos e o custo de cada cabo.
2. Cada roteador **anuncia** (LSA) esses cabos para a rede inteira. No fim, **todos têm o mesmo mapa** da rede (LSDB).
3. Cada roteador roda o Dijkstra **com ele mesmo como origem** e guarda na tabela só o **próximo salto** de cada destino.

**Fontes**
- Vídeo *Algoritmo de Dijkstra - Algoritmos em Grafos*: https://www.youtube.com/watch?v=ftYNBrMVOYQ
- Santiago Valdés Ravelo, *Algoritmo de Dijkstra*, MC558, Unicamp: https://ic.unicamp.br/~santiago/assets/mc558/MC558___12.pdf
- Paulo Feofiloff, *Algoritmo de Dijkstra*, IME-USP: https://users.ime.usp.br/~pf/algoritmos_para_grafos/aulas/dijkstra.html
- freeCodeCamp, *Algoritmo de caminho de custo mínimo de Dijkstra*: https://www.freecodecamp.org/portuguese/news/algoritmo-de-caminho-de-custo-minimo-de-dijkstra-uma-introducao-detalhada-e-visual/
- Princeton COS 461, *Link-State Routing*: https://www.cs.princeton.edu/courses/archive/spr10/cos461/docs/lec10-linkstate.pdf
- University of Michigan EECS 489, *Link-State Routing*: https://web.eecs.umich.edu/~sugih/courses/eecs489/lectures/16-RoutingLS.pdf
- University of Washington CSE 461, *Link State Routing*: https://courses.cs.washington.edu/courses/cse461/08au/lectures/lect15.pdf
- Duke ECE 356, *OSPF*: https://people.duke.edu/~zg70/courses/ECE356/slides/13OSPF.pdf
- CLRS, capítulo "Single-Source Shortest Paths"; Kurose & Ross, *Computer Networking: A Top-Down Approach*, capítulo "Network Layer: Control Plane".

### 2.2 Topologia usada

6 roteadores (R0 a R5) e 9 cabos. Todo roteador tem pelo menos 2 ligações.

```
              4               5               6
      R0 ------------ R1 ------------ R3 ------------ R5
        \            /               / \             /
       2 \          / 1           8 /   \ 2         / 3
          \        /               /     \         /
           \      /               /       \       /
            \    /               /         \     /
              R2 --------------+            R4 -+
                \                          /
                 +----------- 10 ---------+
```

| Roteador | Vizinhos (custo) | Nº de ligações |
|---|---|---|
| R0 | R1(4), R2(2) | 2 |
| R1 | R0(4), R2(1), R3(5) | 3 |
| R2 | R0(2), R1(1), R3(8), R4(10) | 4 |
| R3 | R1(5), R2(8), R4(2), R5(6) | 4 |
| R4 | R2(10), R3(2), R5(3) | 3 |
| R5 | R3(6), R4(3) | 2 |

Os custos foram escolhidos para que o cabo direto quase nunca seja o melhor caminho. Assim o algoritmo precisa corrigir os custos várias vezes, e dá para ver isso acontecendo.

**Custos iniciais** (só os cabos diretos, `-` = sem cabo):

|    | R0 | R1 | R2 | R3 | R4 | R5 |
|----|----|----|----|----|----|----|
| **R0** | 0 | 4 | 2 | - | - | - |
| **R1** | 4 | 0 | 1 | 5 | - | - |
| **R2** | 2 | 1 | 0 | 8 | 10 | - |
| **R3** | - | 5 | 8 | 0 | 2 | 6 |
| **R4** | - | - | 10 | 2 | 0 | 3 |
| **R5** | - | - | - | 6 | 3 | 0 |

**Custos finais** (menor custo entre cada par). A linha de R0 é o que o programa calcula. As outras linhas foram calculadas à parte, trocando a origem, como cada roteador faria no OSPF:

|    | R0 | R1 | R2 | R3 | R4 | R5 |
|----|----|----|----|----|----|----|
| **R0** | 0 | 3 | 2 | 8 | 10 | 13 |
| **R1** | 3 | 0 | 1 | 5 | 7 | 10 |
| **R2** | 2 | 1 | 0 | 6 | 8 | 11 |
| **R3** | 8 | 5 | 6 | 0 | 2 | 5 |
| **R4** | 10 | 7 | 8 | 2 | 0 | 3 |
| **R5** | 13 | 10 | 11 | 5 | 3 | 0 |

Quatro cabos diretos perdem para um caminho indireto:

| Par | Custo direto | Custo final | Caminho mais barato |
|---|---|---|---|
| R0 ↔ R1 | 4 | **3** | R0 → R2 → R1 |
| R2 ↔ R3 | 8 | **6** | R2 → R1 → R3 |
| R2 ↔ R4 | 10 | **8** | R2 → R1 → R3 → R4 |
| R3 ↔ R5 | 6 | **5** | R3 → R4 → R5 |

### 2.3 Como o código foi feito

A estrutura segue a do Bellman-Ford: os mesmos nomes `Enlace`, `Rede`, `createRede` e `printPath`, e o mesmo formato de tabela final. As diferenças:

- **[routers.h](Algoritimo_de_dijkstra/routers.h)**: a `Rede` também guarda uma **matriz de custos** `custo[a][b]`, que é o "mapa" da rede. O Dijkstra usa a matriz para descobrir rapidamente quem é vizinho de quem.
- **[main.c](Algoritimo_de_dijkstra/main.c)**: descreve a topologia em `linksConfig` mostra a topologia, roda o Dijkstra a partir de R0 e imprime a tabela de encaminhamento. A saída tem o mesmo formato da do Bellman-Ford.
- **[routers.c](Algoritimo_de_dijkstra/routers.c)**:
  - `addEnlace` grava cada cabo nos dois sentidos, na lista e na matriz.
  - `printTopologia` mostra os vizinhos de cada roteador e a matriz de custos iniciais.
  - `dijkstra` inicializa os custos e, a cada passo, procura o roteador aberto mais barato, fecha esse roteador e relaxa os vizinhos. No fim de cada passagem, imprime uma linha com o roteador fechado e o custo e o roteador anterior de cada roteador.
  - `printPath` remonta a rota, e `printTabelaEncaminhamento` mostra a tabela final de R0.

### 2.4 Passo a passo (origem R0)

| Passo | Roteador fechado | O que mudou | Observação |
|---|---|---|---|
| início | – | R0 = 0, resto = ∞ | |
| 1 | **R0** (custo 0) | R1 = 4, R2 = 2 | só os vizinhos diretos |
| 2 | **R2** (custo 2) | R1: 4 → **3**, R3 = 10, R4 = 12 | ir por R2 até R1 sai mais barato que o cabo direto |
| 3 | **R1** (custo 3) | R3: 10 → **8** | R0 e R2 já estão fechados e são ignorados |
| 4 | **R3** (custo 8) | R4: 12 → **10**, R5 = 14 | |
| 5 | **R4** (custo 10) | R5: 14 → **13** | |
| 6 | **R5** (custo 13) | nada | todos fechados: fim |

Cada roteador é fechado **uma única vez**, e em ordem crescente de custo (0, 2, 3, 8, 10, 13). É isso que permite ao Dijkstra terminar em só V passos.

### 2.5 Resultado final

```
===============================================================
  ROTEAMENTO COM DIJKSTRA (ESTADO DE ENLACE - ORIGEM: R0)
===============================================================

VIZINHOS DE CADA ROTEADOR (rotas de entrada/saida):
  R0: R1(4), R2(2)   -> 2 enlaces
  R1: R0(4), R2(1), R3(5)   -> 3 enlaces
  R2: R0(2), R1(1), R3(8), R4(10)   -> 4 enlaces
  R3: R1(5), R2(8), R4(2), R5(6)   -> 4 enlaces
  R4: R2(10), R3(2), R5(3)   -> 3 enlaces
  R5: R3(6), R4(3)   -> 2 enlaces

MATRIZ DE CUSTOS INICIAIS ('-' = sem enlace direto):
           R0     R1     R2     R3     R4     R5
R0          0      4      2      -      -      -
R1          4      0      1      5      -      -
R2          2      1      0      8     10      -
R3          -      5      8      0      2      6
R4          -      -     10      2      0      3
R5          -      -      -      6      3      0

RESULTADO DE CADA PASSAGEM (custo/roteador anterior):
  Passagem | Fechou | R0     | R1     | R2     | R3     | R4     | R5
  1        | R0     | 0/R0   | 4/R0   | 2/R0   | INF/-  | INF/-  | INF/-
  2        | R2     | 0/R0   | 3/R2   | 2/R0   | 10/R2  | 12/R2  | INF/-
  3        | R1     | 0/R0   | 3/R2   | 2/R0   | 8/R1   | 12/R2  | INF/-
  4        | R3     | 0/R0   | 3/R2   | 2/R0   | 8/R1   | 10/R3  | 14/R3
  5        | R4     | 0/R0   | 3/R2   | 2/R0   | 8/R1   | 10/R3  | 13/R4
  6        | R5     | 0/R0   | 3/R2   | 2/R0   | 8/R1   | 10/R3  | 13/R4

TABELA DE ENCAMINHAMENTO FINAL:
---------------------------------------------------------------
Destino    | Custo Min.   | Predecessor  | Rota Completa
---------------------------------------------------------------
R0         | 0            | R0           | R0
R1         | 3            | R2           | R0 -> R2 -> R1
R2         | 2            | R0           | R0 -> R2
R3         | 8            | R1           | R0 -> R2 -> R1 -> R3
R4         | 10           | R3           | R0 -> R2 -> R1 -> R3 -> R4
R5         | 13           | R4           | R0 -> R2 -> R1 -> R3 -> R4 -> R5
---------------------------------------------------------------
Status: Arvore de caminhos minimos calculada (todos os custos >= 0).
```

R0 manda todo o tráfego pelo cabo até R2 (próximo salto = R2 para todos os destinos). A mesma saída está salva em [saida.txt](Algoritimo_de_dijkstra/saida.txt).

---

## Parte 3 — Comparação

| | Bellman-Ford | Dijkstra |
|---|---|---|
| Ideia | relaxa **todos** os enlaces, rodada após rodada | fecha **um** roteador por vez, sempre o mais barato |
| Custos negativos | aceita, e detecta ciclo negativo | não aceita |
| Tempo | O(V · E) | O(V²), ou O((V + E) log V) com heap |
| Quanto da rede precisa conhecer | na versão de redes, só os vizinhos | o mapa inteiro |
| Protocolo que usa a ideia | RIP (vetor de distâncias) | OSPF (estado de enlace) |
| Ponto fraco em redes | contagem ao infinito quando um link cai | precisa espalhar o mapa para todos |

---

## Parte 4 — Roteiro para a apresentação

Tempo: cerca de 5 minutos. A ideia é mostrar os programas rodando e explicar rápido. O passo a passo detalhado (seções [1.4](#14-passo-a-passo-origem-r0) e [2.4](#24-passo-a-passo-origem-r0)) e as perguntas abaixo ficam como apoio, caso perguntem.

### Antes de começar
- Copiar as duas pastas para a máquina.
- Em cada pasta, compilar com `gcc -Wall -o routers.exe main.c routers.c`. Se não houver gcc, usar os `routers.exe` que já estão nas pastas.
- Rodar `cls` antes de cada programa, para a tela mostrar só a saída atual.

### Como ler a tabela "RESULTADO DE CADA PASSAGEM"
- Cada linha é uma volta do `for` principal do algoritmo.
- Cada célula mostra `custo/roteador anterior` daquele roteador no fim da volta. Por exemplo, `3/R2` quer dizer "custa 3 e chega vindo de R2". `INF/-` quer dizer que ainda não existe caminho.

### 1. Abertura (30 s)
- "Roteadores precisam escolher o caminho mais barato até cada destino. Vamos mostrar dois algoritmos que fazem isso: o Bellman-Ford, usado no RIP, e o Dijkstra, usado no OSPF."

### 2. Bellman-Ford (2 min)
- **Rodar** o `routers.exe` da pasta do Bellman-Ford.
- **Topologia:** 5 roteadores e 8 cabos; cada roteador tem 3 ou 4 ligações. Mostrar a matriz de custos iniciais.
- **Explicação rápida:** "Cada passagem percorre todos os cabos da rede e, sempre que acha um caminho mais barato, troca. São no máximo V − 1 passagens, e quando uma passagem inteira não muda nada, ele para."
- **Resultado de cada passagem:** na rodada 1 todos os custos já ficaram certos; a rodada 2 não mudou nada, então ele parou.
- **Tabela final:** o cabo direto de R0 até R4 custa 8, mas o caminho R0 → R2 → R3 → R4 custa 6.

### 3. Dijkstra (2 min)
- **Rodar** o `routers.exe` da pasta do Dijkstra.
- **Topologia:** 6 roteadores e 9 cabos; cada roteador tem de 2 a 4 ligações. Mostrar a matriz de custos iniciais.
- **Explicação rápida:** "Cada passagem fecha um roteador, o mais barato que ainda está aberto, e atualiza os vizinhos dele. Por isso são exatamente V passagens, e o custo de um roteador fechado não muda mais. Isso só funciona porque não existe custo negativo."
- **Resultado de cada passagem:** uma linha por roteador fechado. Na passagem 2, R1 cai de 4 para 3 (indo por R2); na passagem 5, R5 cai de 14 para 13 (indo por R4).
- **Tabela final:** R5 é alcançado com custo 13 passando por R2, R1, R3 e R4.

### 4. Fechamento (30 s)
- "O Bellman-Ford aceita custo negativo, mas é mais lento. O Dijkstra é mais rápido, mas exige custos positivos. Nos dois casos o cabo direto quase nunca foi o melhor caminho, e é por isso que os roteadores precisam desses algoritmos."

### Perguntas que podem aparecer
- **Qual a diferença entre as passagens dos dois?** No Bellman-Ford cada passagem percorre todos os cabos, e o mesmo roteador pode melhorar em várias passagens (no máximo V − 1 passagens). No Dijkstra cada passagem fecha um roteador e olha só os vizinhos dele, e o custo de quem foi fechado não muda mais (exatamente V passagens).
- **Por que o Dijkstra não aceita custo negativo?** Porque ele fecha um roteador e nunca mais olha para ele. Um custo negativo encontrado depois poderia baratear esse roteador, e a resposta sairia errada.
- **Por que o Bellman-Ford faz V − 1 rodadas?** Porque um caminho sem repetir roteador tem no máximo V − 1 cabos, e cada rodada garante pelo menos mais um cabo do caminho certo.
- **Como o Bellman-Ford acha ciclo negativo?** Depois das rodadas ele passa por todos os cabos mais uma vez. Se ainda der para melhorar algum custo, existe um ciclo negativo.
- **Qual é mais rápido?** O Dijkstra: O(V²) contra O(V · E) do Bellman-Ford.
- **O que é próximo salto?** O primeiro roteador do caminho. A tabela de um roteador real guarda só isso: "para chegar em R5, mande para R2".
- **O que é a contagem ao infinito?** Um problema do vetor de distâncias. Quando um link cai, dois roteadores podem ficar repassando a informação velha um para o outro e aumentando o custo aos poucos. O RIP limita isso tratando 16 saltos como infinito.
