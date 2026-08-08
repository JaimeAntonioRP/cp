// DSU (Union-Find) con union por tamano + path halving.  O(alpha(n)) amortizado.
#pragma once
#include <bits/stdc++.h>

struct DSU {
    std::vector<int> p, sz;
    int comps;

    explicit DSU(int n) : p(n), sz(n, 1), comps(n) { std::iota(p.begin(), p.end(), 0); }

    int find(int x) {
        while (p[x] != x) x = p[x] = p[p[x]];
        return x;
    }
    bool same(int a, int b) { return find(a) == find(b); }
    int size(int x) { return sz[find(x)]; }

    bool unite(int a, int b) {
        a = find(a), b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) std::swap(a, b);
        p[b] = a;
        sz[a] += sz[b];
        --comps;
        return true;
    }
};
