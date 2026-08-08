// LCA por binary lifting.  Construccion O(n log n), consulta O(log n).
// Tambien da distancia en aristas y el k-esimo ancestro.
#pragma once
#include <bits/stdc++.h>

struct LCA {
    int n, LOG;
    std::vector<std::vector<int>> up;
    std::vector<int> depth;

    LCA(const std::vector<std::vector<int>>& g, int root = 0)
        : n((int)g.size()), LOG(std::__lg(std::max(n, 1)) + 1),
          up(LOG, std::vector<int>(n, -1)), depth(n, 0) {
        // BFS iterativo: evita stack overflow en arboles tipo camino
        std::vector<int> order;
        order.reserve(n);
        std::vector<char> vis(n, 0);
        std::queue<int> q;
        q.push(root);
        vis[root] = 1;
        up[0][root] = -1;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            order.push_back(u);
            for (int v : g[u])
                if (!vis[v]) {
                    vis[v] = 1;
                    depth[v] = depth[u] + 1;
                    up[0][v] = u;
                    q.push(v);
                }
        }
        for (int k = 1; k < LOG; ++k)
            for (int v = 0; v < n; ++v)
                up[k][v] = up[k - 1][v] == -1 ? -1 : up[k - 1][up[k - 1][v]];
    }

    int kth_ancestor(int v, int k) const {
        for (int i = 0; i < LOG && v != -1; ++i)
            if (k >> i & 1) v = up[i][v];
        return v;
    }

    int lca(int a, int b) const {
        if (depth[a] < depth[b]) std::swap(a, b);
        a = kth_ancestor(a, depth[a] - depth[b]);
        if (a == b) return a;
        for (int k = LOG - 1; k >= 0; --k)
            if (up[k][a] != up[k][b]) a = up[k][a], b = up[k][b];
        return up[0][a];
    }

    int dist(int a, int b) const { return depth[a] + depth[b] - 2 * depth[lca(a, b)]; }
};
