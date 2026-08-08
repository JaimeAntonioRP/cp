# Librería

Snippets probados, listos para incluir (`#include "graphs/dijkstra.cpp"`) o para pegar
en el envío al juez. Todos llevan `#pragma once` y su complejidad documentada arriba.

Búscalos desde la terminal con `cpx lib <texto>`.

## data-structures

| Fichero | Qué es | Complejidad |
| --- | --- | --- |
| `dsu.cpp` | Union-Find con unión por tamaño | `O(α(n))` |
| `fenwick.cpp` | BIT: suma en prefijo, k-ésimo elemento | `O(log n)` |
| `segtree.cpp` | Segment tree iterativo, operación genérica | `O(log n)` |
| `lazy_segtree.cpp` | Suma en rango + añadir constante en rango | `O(log n)` |
| `sparse_table.cpp` | RMQ estático (min/max/gcd) | `O(1)` por consulta |
| `ordered_set.cpp` | `pb_ds`: `order_of_key`, `find_by_order` | `O(log n)` |

## graphs

| Fichero | Qué es | Complejidad |
| --- | --- | --- |
| `dijkstra.cpp` | Camino mínimo, pesos ≥ 0, con reconstrucción | `O(m log n)` |
| `bfs01.cpp` | Camino mínimo con pesos 0/1 (deque) | `O(n + m)` |
| `lca.cpp` | LCA por binary lifting, `kth_ancestor`, `dist` | `O(log n)` |
| `scc.cpp` | Tarjan **iterativo** + grafo condensado | `O(n + m)` |
| `toposort.cpp` | Kahn, orden lexicográficamente menor, detecta ciclos | `O(n + m)` |
| `kruskal.cpp` | MST (usa `dsu.cpp`) | `O(m log m)` |

## math

| Fichero | Qué es | Complejidad |
| --- | --- | --- |
| `modint.cpp` | Aritmética modular con operadores sobrecargados | `O(1)` |
| `comb.cpp` | `C(n,k)`, `P(n,k)`, stars & bars, Catalan | `O(1)` tras `O(n)` |
| `sieve.cpp` | Criba lineal, `spf`, factorización, divisores | `O(n)` / `O(log n)` |
| `number_theory.cpp` | `power_mod`, `extgcd`, CRT, Miller-Rabin, Pollard ρ | hasta `10^18` |
| `matrix.cpp` | Exponenciación de matrices (recurrencias lineales) | `O(k³ log e)` |

## strings

| Fichero | Qué es | Complejidad |
| --- | --- | --- |
| `z_function.cpp` | Z-array, búsqueda de patrón | `O(n)` |
| `kmp.cpp` | Prefix function, búsqueda, período mínimo | `O(n)` |
| `hashing.cpp` | Hash doble con base aleatoria (anti-hack) | `O(1)` por substring |
| `manacher.cpp` | Todos los palíndromos, el más largo | `O(n)` |

## misc

| Fichero | Qué es |
| --- | --- |
| `debug.hpp` | `dbg(...)` genérico por `stderr`, solo con `-DLOCAL` |
| `random.cpp` | RNG con semilla del reloj + `custom_hash` anti-colisión |
| `fastio.cpp` | Lector de enteros por buffer para `n ≥ 10⁶` |

---

## Qué añadir después

Cuando te toque un problema que lo pida, añade aquí el snippet en vez de reescribirlo:
árbol de Fenwick 2D, treap / splay, HLD, centroid decomposition, flujo máximo (Dinic),
matching bipartito (Hopcroft-Karp), Aho-Corasick, suffix array, convex hull trick,
Li Chao tree, FFT/NTT, simplex, geometría (envolvente convexa, intersecciones).
