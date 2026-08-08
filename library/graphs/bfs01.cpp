// BFS 0-1: caminos minimos cuando todas las aristas pesan 0 o 1.  O(n + m).
#pragma once
#include <bits/stdc++.h>

std::vector<int> bfs01(int src, const std::vector<std::vector<std::pair<int, int>>>& g) {
    int n = (int)g.size();
    std::vector<int> d(n, INT_MAX);
    std::deque<int> dq;
    d[src] = 0;
    dq.push_back(src);

    while (!dq.empty()) {
        int u = dq.front();
        dq.pop_front();
        for (auto [v, w] : g[u])
            if (d[u] + w < d[v]) {
                d[v] = d[u] + w;
                if (w == 0) dq.push_front(v);
                else        dq.push_back(v);
            }
    }
    return d;
}
