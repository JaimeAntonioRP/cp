/**
 *  Autotest de la libreria: contrasta cada estructura contra fuerza bruta
 *  con datos aleatorios.  No lee entrada; imprime OK/FALLA por modulo.
 *
 *      cpx run library/selftest.cpp
 *
 *  Ejecutalo cada vez que anadas o toques un snippet.
 */
#include <bits/stdc++.h>

#include "data-structures/dsu.cpp"
#include "data-structures/fenwick.cpp"
#include "data-structures/segtree.cpp"
#include "data-structures/lazy_segtree.cpp"
#include "data-structures/sparse_table.cpp"
#include "graphs/dijkstra.cpp"
#include "graphs/bfs01.cpp"
#include "graphs/lca.cpp"
#include "graphs/scc.cpp"
#include "graphs/toposort.cpp"
#include "graphs/kruskal.cpp"
#include "math/modint.cpp"
#include "math/comb.cpp"
#include "math/sieve.cpp"
#include "math/number_theory.cpp"
#include "math/matrix.cpp"
#include "strings/z_function.cpp"
#include "strings/kmp.cpp"
#include "strings/hashing.cpp"
#include "strings/manacher.cpp"

using namespace std;

static int failures = 0;
static mt19937 gen(12345);
static int ri(int l, int r) { return uniform_int_distribution<int>(l, r)(gen); }

static void check(const char* mod, bool ok) {
    printf("%-18s %s\n", mod, ok ? "OK" : "FALLA");
    if (!ok) ++failures;
}

// --------------------------------------------------------------------------
static bool test_dsu() {
    for (int it = 0; it < 200; ++it) {
        int n = ri(1, 12), m = ri(0, 20);
        DSU d(n);
        vector<vector<int>> g(n);
        for (int i = 0; i < m; ++i) {
            int u = ri(0, n - 1), v = ri(0, n - 1);
            d.unite(u, v);
            g[u].push_back(v);
            g[v].push_back(u);
        }
        vector<int> vis(n, -1);
        int comps = 0;
        for (int s = 0; s < n; ++s) {
            if (vis[s] != -1) continue;
            vector<int> st{s};
            vis[s] = comps;
            int cnt = 0;
            while (!st.empty()) {
                int u = st.back(); st.pop_back(); ++cnt;
                for (int v : g[u]) if (vis[v] == -1) { vis[v] = comps; st.push_back(v); }
            }
            for (int v = 0; v < n; ++v) if (vis[v] == comps && d.size(v) != cnt) return false;
            ++comps;
        }
        if (d.comps != comps) return false;
        for (int u = 0; u < n; ++u)
            for (int v = 0; v < n; ++v)
                if (d.same(u, v) != (vis[u] == vis[v])) return false;
    }
    return true;
}

static bool test_fenwick() {
    for (int it = 0; it < 200; ++it) {
        int n = ri(1, 30);
        vector<long long> a(n, 0);
        Fenwick<long long> f(n);
        for (int q = 0; q < 60; ++q) {
            if (ri(0, 1)) {
                int i = ri(0, n - 1), v = ri(0, 9);
                a[i] += v;
                f.add(i, v);
            } else {
                int l = ri(0, n), r = ri(0, n);
                if (l > r) swap(l, r);
                long long s = 0;
                for (int i = l; i < r; ++i) s += a[i];
                if (f.sum(l, r) != s) return false;
            }
        }
        // kth sobre valores no negativos
        long long total = accumulate(a.begin(), a.end(), 0LL);
        for (long long k = 0; k < min(total, 20LL); ++k) {
            int pos = f.kth(k);
            long long acc = 0;
            int expect = 0;
            for (int i = 0; i < n; ++i) {
                acc += a[i];
                if (acc > k) { expect = i; break; }
            }
            if (pos != expect) return false;
        }
    }
    return true;
}

static bool test_segtree() {
    for (int it = 0; it < 200; ++it) {
        int n = ri(1, 30);
        vector<long long> a(n);
        for (auto& x : a) x = ri(-20, 20);
        auto add = [](long long x, long long y) { return x + y; };
        auto mn  = [](long long x, long long y) { return min(x, y); };
        auto sSum = make_seg<long long>(a, 0LL, add);
        auto sMin = make_seg<long long>(a, (long long)4e18, mn);
        for (int q = 0; q < 40; ++q) {
            if (ri(0, 1)) {
                int i = ri(0, n - 1);
                long long v = ri(-20, 20);
                a[i] = v;
                sSum.set(i, v);
                sMin.set(i, v);
            } else {
                int l = ri(0, n), r = ri(0, n);
                if (l > r) swap(l, r);
                long long s = 0, m = 4e18;
                for (int i = l; i < r; ++i) s += a[i], m = min(m, a[i]);
                if (sSum.query(l, r) != s) return false;
                if (sMin.query(l, r) != m) return false;
            }
        }
    }
    return true;
}

static bool test_lazy() {
    for (int it = 0; it < 200; ++it) {
        int n = ri(1, 30);
        vector<long long> a(n);
        for (auto& x : a) x = ri(-20, 20);
        LazySeg st(a);
        for (int q = 0; q < 40; ++q) {
            int l = ri(0, n), r = ri(0, n);
            if (l > r) swap(l, r);
            if (ri(0, 1)) {
                long long v = ri(-10, 10);
                for (int i = l; i < r; ++i) a[i] += v;
                st.update(l, r, v);
            } else {
                long long s = 0;
                for (int i = l; i < r; ++i) s += a[i];
                if (st.query(l, r) != s) return false;
            }
        }
    }
    return true;
}

static bool test_sparse() {
    for (int it = 0; it < 200; ++it) {
        int n = ri(1, 40);
        vector<int> a(n);
        for (auto& x : a) x = ri(-50, 50);
        auto mn = [](int x, int y) { return min(x, y); };
        auto sp = make_sparse<int>(a, mn);
        for (int q = 0; q < 40; ++q) {
            int l = ri(0, n - 1), r = ri(l + 1, n);
            int m = INT_MAX;
            for (int i = l; i < r; ++i) m = min(m, a[i]);
            if (sp.query(l, r) != m) return false;
        }
    }
    return true;
}

static bool test_dijkstra_bfs01() {
    const long long LNF = 4e18;
    for (int it = 0; it < 150; ++it) {
        int n = ri(1, 9), m = ri(0, 20);
        vector<vector<pair<int, int>>> g(n), g01(n);
        vector<vector<long long>> fw(n, vector<long long>(n, LNF));
        for (int i = 0; i < n; ++i) fw[i][i] = 0;
        for (int i = 0; i < m; ++i) {
            int u = ri(0, n - 1), v = ri(0, n - 1), w = ri(0, 9);
            g[u].emplace_back(v, w);
            g01[u].emplace_back(v, w > 0 ? 1 : 0);
            fw[u][v] = min(fw[u][v], (long long)w);
        }
        for (int k = 0; k < n; ++k)
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    if (fw[i][k] < LNF && fw[k][j] < LNF)
                        fw[i][j] = min(fw[i][j], fw[i][k] + fw[k][j]);

        int src = ri(0, n - 1);
        vector<int> par;
        auto d = dijkstra(src, g, &par);
        for (int v = 0; v < n; ++v) if (d[v] != fw[src][v]) return false;

        // el camino reconstruido debe existir y pesar exactamente d[dst]
        for (int dst = 0; dst < n; ++dst) {
            auto path = restore_path(src, dst, par, d);
            if (d[dst] == LNF) { if (!path.empty()) return false; continue; }
            if (path.empty() || path.front() != src || path.back() != dst) return false;
            long long w = 0;
            for (size_t i = 0; i + 1 < path.size(); ++i) {
                long long best = LNF;
                for (auto [v, ww] : g[path[i]]) if (v == path[i + 1]) best = min(best, (long long)ww);
                if (best == LNF) return false;
                w += best;
            }
            if (w != d[dst]) return false;
        }

        auto d01 = bfs01(src, g01);
        auto dref = dijkstra(src, g01);
        for (int v = 0; v < n; ++v) {
            long long got = d01[v] == INT_MAX ? LNF : d01[v];
            if (got != dref[v]) return false;
        }
    }
    return true;
}

static bool test_lca() {
    for (int it = 0; it < 200; ++it) {
        int n = ri(1, 30);
        vector<vector<int>> g(n);
        vector<int> par(n, -1), dep(n, 0);
        for (int v = 1; v < n; ++v) {
            int p = ri(0, v - 1);
            par[v] = p;
            dep[v] = dep[p] + 1;
            g[p].push_back(v);
            g[v].push_back(p);
        }
        LCA lca(g, 0);
        for (int a = 0; a < n; ++a) {
            if (lca.depth[a] != dep[a]) return false;
            for (int b = 0; b < n; ++b) {
                int x = a, y = b;
                while (dep[x] > dep[y]) x = par[x];
                while (dep[y] > dep[x]) y = par[y];
                while (x != y) x = par[x], y = par[y];
                if (lca.lca(a, b) != x) return false;
                if (lca.dist(a, b) != dep[a] + dep[b] - 2 * dep[x]) return false;
            }
            for (int k = 0; k <= dep[a]; ++k) {
                int x = a;
                for (int i = 0; i < k; ++i) x = par[x];
                if (lca.kth_ancestor(a, k) != x) return false;
            }
        }
    }
    return true;
}

static bool test_scc() {
    for (int it = 0; it < 200; ++it) {
        int n = ri(1, 9), m = ri(0, 20);
        SCC s(n);
        vector<vector<char>> reach(n, vector<char>(n, 0));
        for (int i = 0; i < n; ++i) reach[i][i] = 1;
        for (int i = 0; i < m; ++i) {
            int u = ri(0, n - 1), v = ri(0, n - 1);
            s.add_edge(u, v);
            reach[u][v] = 1;
        }
        for (int k = 0; k < n; ++k)
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    if (reach[i][k] && reach[k][j]) reach[i][j] = 1;
        s.run();
        for (int u = 0; u < n; ++u)
            for (int v = 0; v < n; ++v) {
                bool same = reach[u][v] && reach[v][u];
                if ((s.comp[u] == s.comp[v]) != same) return false;
            }
        // los ids deben ir en orden topologico inverso del condensado
        for (int u = 0; u < n; ++u)
            for (auto v : s.g[u])
                if (s.comp[u] != s.comp[v] && s.comp[u] < s.comp[v]) return false;
    }
    return true;
}

static bool test_toposort() {
    for (int it = 0; it < 200; ++it) {
        int n = ri(1, 12);
        bool acyclic = ri(0, 1);
        vector<vector<int>> g(n);
        int m = ri(0, 20);
        for (int i = 0; i < m; ++i) {
            int u = ri(0, n - 1), v = ri(0, n - 1);
            if (acyclic) { if (u == v) continue; if (u > v) swap(u, v); }
            g[u].push_back(v);
        }
        auto ord = toposort(g);
        if (acyclic) {
            if ((int)ord.size() != n) return false;
            vector<int> at(n);
            for (int i = 0; i < n; ++i) at[ord[i]] = i;
            for (int u = 0; u < n; ++u)
                for (int v : g[u]) if (at[u] > at[v]) return false;
        } else if (!ord.empty()) {
            if ((int)ord.size() != n) return false;
            vector<int> at(n);
            for (int i = 0; i < n; ++i) at[ord[i]] = i;
            for (int u = 0; u < n; ++u)
                for (int v : g[u]) if (at[u] > at[v]) return false;
        }
    }
    return true;
}

static bool test_kruskal() {
    for (int it = 0; it < 200; ++it) {
        int n = ri(1, 10);
        vector<Edge> es;
        vector<vector<long long>> w(n, vector<long long>(n, LLONG_MAX));
        for (int i = 0; i < ri(0, 25); ++i) {
            int u = ri(0, n - 1), v = ri(0, n - 1);
            if (u == v) continue;
            long long ww = ri(1, 20);
            es.push_back({u, v, ww});
            w[u][v] = min(w[u][v], ww);
            w[v][u] = min(w[v][u], ww);
        }
        auto [total, used] = kruskal(n, es);

        // Prim por componentes como referencia
        vector<char> vis(n, 0);
        long long ref = 0;
        for (int s = 0; s < n; ++s) {
            if (vis[s]) continue;
            vis[s] = 1;
            while (true) {
                long long best = LLONG_MAX;
                int bv = -1;
                for (int u = 0; u < n; ++u) {
                    if (!vis[u]) continue;
                    for (int v = 0; v < n; ++v)
                        if (!vis[v] && w[u][v] < best) best = w[u][v], bv = v;
                }
                if (bv == -1) break;
                vis[bv] = 1;
                ref += best;
            }
        }
        if (total != ref) return false;
    }
    return true;
}

static bool test_modint_comb() {
    const long long MOD = mint::mod();
    Comb cb(60);
    vector<vector<long long>> pas(61, vector<long long>(61, 0));
    for (int i = 0; i <= 60; ++i) {
        pas[i][0] = 1;
        for (int j = 1; j <= i; ++j) pas[i][j] = (pas[i - 1][j - 1] + pas[i - 1][j]) % MOD;
    }
    for (int n = 0; n <= 60; ++n)
        for (int k = 0; k <= 60; ++k)
            if ((long long)cb.C(n, k).v != (k <= n ? pas[n][k] : 0)) return false;

    for (int it = 0; it < 500; ++it) {
        long long x = ri(1, 1000000), y = ri(1, 1000000);
        mint a(x), b(y);
        if ((long long)(a + b).v != (x + y) % MOD) return false;
        if ((long long)(a * b).v != x * y % MOD) return false;
        if ((a / b * b) != a) return false;
        if ((a * a.inv()) != mint(1)) return false;
        long long e = ri(0, 50), r = 1;
        for (int i = 0; i < e; ++i) r = r * (x % MOD) % MOD;
        if ((long long)a.pow(e).v != r) return false;
    }
    // Catalan: 1, 1, 2, 5, 14, 42, 132
    int cat[] = {1, 1, 2, 5, 14, 42, 132};
    for (int i = 0; i <= 6; ++i) if ((long long)cb.catalan(i).v != cat[i]) return false;
    return true;
}

static bool test_sieve() {
    const int N = 3000;
    Sieve s(N);
    for (int x = 2; x <= N; ++x) {
        bool p = true;
        for (int d = 2; 1LL * d * d <= x; ++d) if (x % d == 0) { p = false; break; }
        if (s.is_prime(x) != p) return false;
        long long prod = 1;
        for (auto [q, c] : s.factor(x)) {
            if (!s.is_prime(q)) return false;
            for (int i = 0; i < c; ++i) prod *= q;
        }
        if (prod != x) return false;
    }
    for (int it = 0; it < 200; ++it) {
        int x = ri(1, N);
        auto d = s.divisors(x);
        vector<int> ref;
        for (int i = 1; i <= x; ++i) if (x % i == 0) ref.push_back(i);
        if (d != ref) return false;
    }
    return true;
}

static bool test_number_theory() {
    for (int x = 2; x <= 2000; ++x) {
        bool p = true;
        for (int d = 2; 1LL * d * d <= x; ++d) if (x % d == 0) { p = false; break; }
        if (is_prime(x) != p) return false;
    }
    if (!is_prime(1000000007LL) || !is_prime(998244353LL)) return false;
    if (is_prime(1000000007LL * 3)) return false;

    for (int it = 0; it < 40; ++it) {
        long long x = (long long)ri(2, 1000000) * ri(2, 1000000);
        auto f = factorize(x);
        long long prod = 1;
        for (auto [p, c] : f) {
            if (!is_prime(p)) return false;
            for (int i = 0; i < c; ++i) prod *= p;
        }
        if (prod != x) return false;
    }
    for (int it = 0; it < 500; ++it) {
        long long a = ri(-1000, 1000), b = ri(-1000, 1000);
        if (a == 0 && b == 0) continue;
        long long x, y, g = extgcd(a, b, x, y);
        if (a * x + b * y != g) return false;
        if (g != __gcd(llabs(a), llabs(b))) return false;
    }
    for (int it = 0; it < 500; ++it) {
        long long m = ri(2, 500), a = ri(1, (int)m - 1);
        long long inv = inv_mod(a, m);
        if (__gcd(a, m) == 1) {
            if (inv < 0 || a * inv % m != 1) return false;
        } else if (inv != -1) return false;
    }
    for (int it = 0; it < 500; ++it) {
        long long m1 = ri(2, 40), m2 = ri(2, 40);
        long long r1 = ri(0, (int)m1 - 1), r2 = ri(0, (int)m2 - 1);
        auto [r, m] = crt(r1, m1, r2, m2);
        bool exists = false;
        long long lcm = m1 / __gcd(m1, m2) * m2;
        for (long long x = 0; x < lcm; ++x)
            if (x % m1 == r1 && x % m2 == r2) { exists = true; break; }
        if (!exists) { if (r != -1) return false; continue; }
        if (r < 0 || m != lcm || r % m1 != r1 || r % m2 != r2) return false;
    }
    for (int it = 0; it < 300; ++it) {
        long long b = ri(0, 1000), e = ri(0, 30), m = ri(2, 1000), r = 1 % m;
        for (int i = 0; i < e; ++i) r = r * b % m;
        if (power_mod(b, e, m) != r) return false;
    }
    return true;
}

static bool test_matrix() {
    // [[1,1],[1,0]]^n == [[F(n+1), F(n)], [F(n), F(n-1)]]  con F(0)=0, F(1)=1
    vector<long long> F(200);
    F[0] = 0; F[1] = 1;
    for (int i = 2; i < 200; ++i) F[i] = (F[i - 1] + F[i - 2]) % mint::mod();

    Matrix f(2);
    f.a[0][0] = 1; f.a[0][1] = 1; f.a[1][0] = 1; f.a[1][1] = 0;
    for (int n = 1; n <= 150; ++n) {
        Matrix p = f.pow(n);
        if (p.a[0][0] != mint(F[n + 1])) return false;
        if (p.a[0][1] != mint(F[n]))     return false;
        if (p.a[1][0] != mint(F[n]))     return false;
        if (p.a[1][1] != mint(F[n - 1])) return false;
    }
    if (f.pow(0).a[0][0] != mint(1) || f.pow(0).a[0][1] != mint(0)) return false;
    if (Matrix::identity(3).pow(1000).a[2][2] != mint(1)) return false;
    return true;
}

static bool test_strings() {
    const string alpha = "ab";
    for (int it = 0; it < 400; ++it) {
        int n = ri(1, 14);
        string s;
        for (int i = 0; i < n; ++i) s += alpha[ri(0, (int)alpha.size() - 1)];

        // z-function
        auto z = z_function(s);
        for (int i = 1; i < n; ++i) {
            int k = 0;
            while (i + k < n && s[k] == s[i + k]) ++k;
            if (z[i] != k) return false;
        }
        if (z[0] != n) return false;

        // prefix function
        auto pi = prefix_function(s);
        for (int i = 0; i < n; ++i) {
            int best = 0;
            for (int len = 1; len <= i; ++len)
                if (s.substr(0, len) == s.substr(i - len + 1, len)) best = len;
            if (pi[i] != best) return false;
        }

        // busqueda de patron
        int pl = ri(1, n);
        string pat = s.substr(ri(0, n - pl), pl);
        auto hits = kmp_search(pat, s);
        vector<int> ref;
        for (int i = 0; i + pl <= n; ++i) if (s.compare(i, pl, pat) == 0) ref.push_back(i);
        if (hits != ref) return false;

        // periodo minimo
        int per = min_period(s);
        bool okper = (n % per == 0);
        for (int i = 0; okper && i < n; ++i) if (s[i] != s[i % per]) okper = false;
        if (!okper) return false;
        for (int p = 1; p < per; ++p) {
            if (n % p) continue;
            bool good = true;
            for (int i = 0; i < n; ++i) if (s[i] != s[i % p]) { good = false; break; }
            if (good) return false;   // habia uno menor
        }

        // hashing
        StrHash h(s);
        for (int q = 0; q < 30; ++q) {
            int l1 = ri(0, n - 1), r1 = ri(l1 + 1, n);
            int len = r1 - l1;
            int l2 = ri(0, n - len), r2 = l2 + len;
            bool eq = (s.compare(l1, len, s, l2, len) == 0);
            if ((h.get(l1, r1) == h.get(l2, r2)) != eq) return false;
        }

        // manacher
        int best = 0;
        for (int i = 0; i < n; ++i)
            for (int j = i; j < n; ++j) {
                string t = s.substr(i, j - i + 1);
                string r = t;
                reverse(r.begin(), r.end());
                if (t == r) best = max(best, (int)t.size());
            }
        if (longest_palindrome(s) != best) return false;
    }
    return true;
}

int main() {
    printf("--- autotest de la libreria ---\n");
    check("dsu",           test_dsu());
    check("fenwick",       test_fenwick());
    check("segtree",       test_segtree());
    check("lazy_segtree",  test_lazy());
    check("sparse_table",  test_sparse());
    check("dijkstra/bfs01", test_dijkstra_bfs01());
    check("lca",           test_lca());
    check("scc",           test_scc());
    check("toposort",      test_toposort());
    check("kruskal",       test_kruskal());
    check("modint/comb",   test_modint_comb());
    check("sieve",         test_sieve());
    check("number_theory", test_number_theory());
    check("matrix",        test_matrix());
    check("strings",       test_strings());
    printf("--- %s ---\n", failures ? "HAY FALLOS" : "todo correcto");
    return failures ? 1 : 0;
}
