/**
 *  Autor : JaimeAntonioRP
 *  Problema: 20C - Dijkstra?
 *  Enlace  : https://codeforces.com/problemset/problem/20/C
 *  Idea    : dijkstra desde el 1 guardando padres, y reconstruir el camino
 *            hasta n. Piden el CAMINO, no la distancia.
 */
#include <bits/stdc++.h>
using namespace std;

using ll  = long long;
using ull = unsigned long long;
using ld  = long double;
using pii = pair<int, int>;
using pll = pair<ll, ll>;
template <class T> using vc  = vector<T>;
template <class T> using vvc = vector<vector<T>>;
template <class T> using minpq = priority_queue<T, vector<T>, greater<T>>;

#define all(x)  (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#define sz(x)   ((int)(x).size())
#define pb      push_back
#define eb      emplace_back
#define fi      first
#define se      second

#ifdef LOCAL
#include "debug.hpp"
#else
#define dbg(...)  ((void)0)
#define dbgr(...) ((void)0)   // sin esto, un dbgr() olvidado = CE en el juez
#endif

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

constexpr int INF = 1'000'000'007;

void solve() {
    int n, m;
    cin >> n >> m;

    // g[u] = lista de (vecino, peso).  Es el formato que espera el snippet:
    // vector<vector<pair<int,int>>>
    vvc<pii> g(n);
    for (int i = 0; i < m; ++i) {
        int a, b, w;
        cin >> a >> b >> w;
        --a, --b;                 // el enunciado va de 1..n, yo trabajo en 0..n-1
        g[a].eb(b, w);
        g[b].eb(a, w);            // no dirigido: las dos direcciones
    }

    vc<int> par;                          // se rellena dentro de dijkstra
    vc<ll>  d = dijkstra(0, g, &par);     // 0 = vertice 1 del enunciado
    dbg(d[n - 1]);

    vc<int> path = restore_path(0, n - 1, par, d);
    if (path.empty()) {                   // n inalcanzable desde 1
        cout << -1 << "\n";
        return;
    }

    for (int i = 0; i < sz(path); ++i)
        cout << path[i] + 1 << " \n"[i + 1 == sz(path)];   // +1: volver a 1-indexed
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    solve();
    return 0;
}
