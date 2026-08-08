// Fenwick / BIT.  Indices 0-based en la interfaz, rangos semiabiertos [l, r).
// add: O(log n), sum: O(log n), kth: O(log n).
#pragma once
#include <bits/stdc++.h>

template <class T>
struct Fenwick {
    int n;
    std::vector<T> t;

    explicit Fenwick(int n_ = 0) : n(n_), t(n_ + 1, T{}) {}

    void add(int i, T v) {              // a[i] += v
        for (++i; i <= n; i += i & -i) t[i] += v;
    }
    T sum(int i) const {                // suma de [0, i)
        T s{};
        for (; i > 0; i -= i & -i) s += t[i];
        return s;
    }
    T sum(int l, int r) const { return sum(r) - sum(l); }   // [l, r)

    // Menor indice pos tal que sum(pos + 1) > k.  Requiere valores >= 0.
    int kth(T k) const {
        int pos = 0;
        for (int pw = 1 << std::__lg(std::max(n, 1)); pw; pw >>= 1)
            if (pos + pw <= n && t[pos + pw] <= k) pos += pw, k -= t[pos];
        return pos;                      // 0-based
    }
};
