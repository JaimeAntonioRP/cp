// MST por Kruskal.  O(m log m).  Requiere DSU (../data-structures/dsu.cpp).
#pragma once
#include <bits/stdc++.h>
#include "data-structures/dsu.cpp"

struct Edge {
    int u, v;
    long long w;
    bool operator<(const Edge& o) const { return w < o.w; }
};

// Devuelve {peso total, aristas usadas}.  Si el grafo es disconexo, devuelve el bosque.
std::pair<long long, std::vector<Edge>> kruskal(int n, std::vector<Edge> es) {
    std::sort(es.begin(), es.end());
    DSU dsu(n);
    long long total = 0;
    std::vector<Edge> used;
    for (const Edge& e : es)
        if (dsu.unite(e.u, e.v)) {
            total += e.w;
            used.push_back(e);
        }
    return {total, used};
}
