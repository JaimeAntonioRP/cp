// Sparse table para operaciones idempotentes (min, max, gcd, and, or).
// Construccion O(n log n), query O(1).  El arreglo debe ser inmutable.
#pragma once
#include <bits/stdc++.h>

template <class T, class Op>
struct SparseTable {
    Op op;
    std::vector<std::vector<T>> st;

    SparseTable(const std::vector<T>& a, Op op_) : op(op_) {
        int n = (int)a.size(), K = std::__lg(std::max(n, 1)) + 1;
        st.assign(K, std::vector<T>(n));
        st[0] = a;
        for (int k = 1; k < K; ++k)
            for (int i = 0; i + (1 << k) <= n; ++i)
                st[k][i] = op(st[k - 1][i], st[k - 1][i + (1 << (k - 1))]);
    }

    T query(int l, int r) const {       // [l, r), requiere l < r
        int k = std::__lg(r - l);
        return op(st[k][l], st[k][r - (1 << k)]);
    }
};

template <class T, class Op>
SparseTable<T, Op> make_sparse(const std::vector<T>& a, Op op) {
    return SparseTable<T, Op>(a, op);
}
