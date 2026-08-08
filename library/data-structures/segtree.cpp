// Segment tree iterativo (update puntual, query en rango [l, r)).
// La operacion debe ser asociativa y `e` su elemento neutro.
// Construccion O(n), update/query O(log n).
//
//   auto st = make_seg<long long>(a, 0LL, [](long long x, long long y){ return x + y; });
//   st.set(i, v);  st.query(l, r);
#pragma once
#include <bits/stdc++.h>

template <class T, class Op>
struct SegTree {
    int n;
    T e;
    Op op;
    std::vector<T> t;

    SegTree(int n_, T e_, Op op_) : n(n_), e(e_), op(op_), t(2 * n_, e_) {}

    SegTree(const std::vector<T>& a, T e_, Op op_)
        : n((int)a.size()), e(e_), op(op_), t(2 * a.size(), e_) {
        for (int i = 0; i < n; ++i) t[n + i] = a[i];
        for (int i = n - 1; i > 0; --i) t[i] = op(t[2 * i], t[2 * i + 1]);
    }

    T get(int i) const { return t[i + n]; }

    void set(int i, T v) {
        for (t[i += n] = v; i >>= 1;) t[i] = op(t[2 * i], t[2 * i + 1]);
    }

    T query(int l, int r) const {       // [l, r)
        T resl = e, resr = e;
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) resl = op(resl, t[l++]);
            if (r & 1) resr = op(t[--r], resr);
        }
        return op(resl, resr);
    }
};

template <class T, class Op>
SegTree<T, Op> make_seg(const std::vector<T>& a, T e, Op op) {
    return SegTree<T, Op>(a, e, op);
}
