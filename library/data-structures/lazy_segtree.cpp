// Segment tree con lazy propagation: suma en rango + sumar constante en rango.
// Construccion O(n), update/query O(log n).
//
//   LazySeg st(a);        // a es vector<long long>
//   st.update(l, r, v);   // a[i] += v para i en [l, r)
//   st.query(l, r);       // suma de [l, r)
#pragma once
#include <bits/stdc++.h>

struct LazySeg {
    using ll = long long;
    int n;
    std::vector<ll> t, lz;

    explicit LazySeg(int n_) : n(n_), t(4 * n_, 0), lz(4 * n_, 0) {}
    explicit LazySeg(const std::vector<ll>& a) : n((int)a.size()), t(4 * a.size(), 0), lz(4 * a.size(), 0) {
        build(1, 0, n - 1, a);
    }

    void build(int v, int l, int r, const std::vector<ll>& a) {
        if (l == r) { t[v] = a[l]; return; }
        int m = (l + r) / 2;
        build(2 * v, l, m, a);
        build(2 * v + 1, m + 1, r, a);
        t[v] = t[2 * v] + t[2 * v + 1];
    }

    void apply(int v, int l, int r, ll x) {
        t[v] += x * (r - l + 1);
        lz[v] += x;
    }
    void push(int v, int l, int r) {
        if (lz[v] == 0) return;
        int m = (l + r) / 2;
        apply(2 * v, l, m, lz[v]);
        apply(2 * v + 1, m + 1, r, lz[v]);
        lz[v] = 0;
    }

    void update(int v, int l, int r, int ql, int qr, ll x) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) { apply(v, l, r, x); return; }
        push(v, l, r);
        int m = (l + r) / 2;
        update(2 * v, l, m, ql, qr, x);
        update(2 * v + 1, m + 1, r, ql, qr, x);
        t[v] = t[2 * v] + t[2 * v + 1];
    }
    ll query(int v, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return t[v];
        push(v, l, r);
        int m = (l + r) / 2;
        return query(2 * v, l, m, ql, qr) + query(2 * v + 1, m + 1, r, ql, qr);
    }

    // interfaz publica, rango semiabierto [l, r)
    void update(int l, int r, ll x) { if (l < r) update(1, 0, n - 1, l, r - 1, x); }
    ll query(int l, int r) { return l < r ? query(1, 0, n - 1, l, r - 1) : 0; }
};
