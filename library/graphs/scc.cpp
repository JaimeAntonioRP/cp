// Componentes fuertemente conexas (Tarjan iterativo — sin riesgo de stack overflow).
// comp[v] = id de su SCC.  Los ids salen en orden topologico INVERSO del grafo condensado.
// O(n + m).
#pragma once
#include <bits/stdc++.h>

struct SCC {
    int n, timer = 0, ncomp = 0;
    std::vector<std::vector<int>> g;
    std::vector<int> low, num, comp, stk;
    std::vector<char> onstk;

    explicit SCC(int n_) : n(n_), g(n_), low(n_, 0), num(n_, -1), comp(n_, -1), onstk(n_, 0) {}

    void add_edge(int u, int v) { g[u].push_back(v); }

    void run() {
        std::vector<std::pair<int, int>> call;   // (nodo, indice de arista)
        for (int s = 0; s < n; ++s) {
            if (num[s] != -1) continue;
            call.emplace_back(s, 0);
            num[s] = low[s] = timer++;
            stk.push_back(s);
            onstk[s] = 1;
            while (!call.empty()) {
                auto& [u, i] = call.back();
                if (i < (int)g[u].size()) {
                    int v = g[u][i++];
                    if (num[v] == -1) {
                        num[v] = low[v] = timer++;
                        stk.push_back(v);
                        onstk[v] = 1;
                        call.emplace_back(v, 0);
                    } else if (onstk[v]) {
                        low[u] = std::min(low[u], num[v]);
                    }
                } else {
                    if (low[u] == num[u]) {
                        while (true) {
                            int w = stk.back();
                            stk.pop_back();
                            onstk[w] = 0;
                            comp[w] = ncomp;
                            if (w == u) break;
                        }
                        ++ncomp;
                    }
                    int child = u;
                    call.pop_back();
                    if (!call.empty()) {
                        int p = call.back().first;
                        low[p] = std::min(low[p], low[child]);
                    }
                }
            }
        }
    }

    // Grafo condensado (sin aristas duplicadas).
    std::vector<std::vector<int>> condense() const {
        std::vector<std::set<int>> s(ncomp);
        for (int u = 0; u < n; ++u)
            for (int v : g[u])
                if (comp[u] != comp[v]) s[comp[u]].insert(comp[v]);
        std::vector<std::vector<int>> h(ncomp);
        for (int i = 0; i < ncomp; ++i) h[i].assign(s[i].begin(), s[i].end());
        return h;
    }
};
