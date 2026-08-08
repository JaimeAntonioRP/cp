// Combinatoria con factoriales precalculados.  Requiere modint.cpp.
// init O(n), consultas O(1).
#pragma once
#include <bits/stdc++.h>
#include "math/modint.cpp"

struct Comb {
    std::vector<mint> f, invf;

    explicit Comb(int n) : f(n + 1), invf(n + 1) {
        f[0] = 1;
        for (int i = 1; i <= n; ++i) f[i] = f[i - 1] * i;
        invf[n] = f[n].inv();
        for (int i = n; i > 0; --i) invf[i - 1] = invf[i] * i;
    }

    mint fact(int n) const { return f[n]; }
    mint C(int n, int k) const { return (k < 0 || k > n || n < 0) ? mint(0) : f[n] * invf[k] * invf[n - k]; }
    mint P(int n, int k) const { return (k < 0 || k > n || n < 0) ? mint(0) : f[n] * invf[n - k]; }
    // Numero de formas de poner n bolas identicas en k cajas (cajas pueden quedar vacias)
    mint stars_bars(int n, int k) const { return C(n + k - 1, k - 1); }
    mint catalan(int n) const { return C(2 * n, n) - C(2 * n, n + 1); }
};
