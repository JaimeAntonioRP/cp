// Matriz cuadrada con exponenciacion rapida (recurrencias lineales).  Requiere modint.cpp.
// pow: O(k^3 log e).
#pragma once
#include <bits/stdc++.h>
#include "math/modint.cpp"

struct Matrix {
    int n;
    std::vector<std::vector<mint>> a;

    explicit Matrix(int n_) : n(n_), a(n_, std::vector<mint>(n_, 0)) {}

    static Matrix identity(int k) {
        Matrix I(k);
        for (int i = 0; i < k; ++i) I.a[i][i] = 1;
        return I;
    }

    Matrix operator*(const Matrix& o) const {
        Matrix r(n);
        for (int i = 0; i < n; ++i)
            for (int k = 0; k < n; ++k) {
                if (a[i][k] == mint(0)) continue;
                for (int j = 0; j < n; ++j) r.a[i][j] += a[i][k] * o.a[k][j];
            }
        return r;
    }

    Matrix pow(long long e) const {
        Matrix r = identity(n), b = *this;
        for (; e > 0; e >>= 1, b = b * b)
            if (e & 1) r = r * b;
        return r;
    }
};
