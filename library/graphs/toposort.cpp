// Orden topologico (Kahn).  Devuelve vector vacio si el grafo tiene ciclo.  O(n + m).
#pragma once
#include <bits/stdc++.h>

std::vector<int> toposort(const std::vector<std::vector<int>>& g) {
    int n = (int)g.size();
    std::vector<int> indeg(n, 0), order;
    for (int u = 0; u < n; ++u)
        for (int v : g[u]) ++indeg[v];

    std::priority_queue<int, std::vector<int>, std::greater<int>> pq;  // lexicograficamente menor
    for (int v = 0; v < n; ++v)
        if (indeg[v] == 0) pq.push(v);

    order.reserve(n);
    while (!pq.empty()) {
        int u = pq.top();
        pq.pop();
        order.push_back(u);
        for (int v : g[u])
            if (--indeg[v] == 0) pq.push(v);
    }
    return (int)order.size() == n ? order : std::vector<int>{};
}
