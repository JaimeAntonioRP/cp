// Dijkstra con priority_queue.  O(m log n).  Pesos no negativos.
// Devuelve dist[] (LNF si inalcanzable) y opcionalmente par[] para reconstruir.
#pragma once
#include <bits/stdc++.h>

std::vector<long long> dijkstra(int src,
                                const std::vector<std::vector<std::pair<int, int>>>& g,
                                std::vector<int>* par = nullptr) {
    using ll = long long;
    const ll LNF = 4e18;
    int n = (int)g.size();
    std::vector<ll> d(n, LNF);
    if (par) par->assign(n, -1);

    std::priority_queue<std::pair<ll, int>, std::vector<std::pair<ll, int>>,
                        std::greater<std::pair<ll, int>>> pq;
    d[src] = 0;
    pq.emplace(0, src);

    while (!pq.empty()) {
        auto [du, u] = pq.top();
        pq.pop();
        if (du != d[u]) continue;                 // entrada obsoleta
        for (auto [v, w] : g[u])
            if (d[u] + w < d[v]) {
                d[v] = d[u] + w;
                if (par) (*par)[v] = u;
                pq.emplace(d[v], v);
            }
    }
    return d;
}

// Reconstruye el camino src -> dst usando par de dijkstra (vacio si no existe).
inline std::vector<int> restore_path(int src, int dst, const std::vector<int>& par,
                                     const std::vector<long long>& d) {
    const long long LNF = 4e18;
    if (d[dst] == LNF) return {};
    std::vector<int> path;
    for (int v = dst; v != -1; v = par[v]) path.push_back(v);
    std::reverse(path.begin(), path.end());
    return path.front() == src ? path : std::vector<int>{};
}
