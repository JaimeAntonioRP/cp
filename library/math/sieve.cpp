// Criba lineal: primos + menor factor primo (spf) => factorizacion en O(log n).
#pragma once
#include <bits/stdc++.h>

struct Sieve {
    std::vector<int> spf, primes;

    explicit Sieve(int n) : spf(n + 1, 0) {
        for (int i = 2; i <= n; ++i) {
            if (spf[i] == 0) { spf[i] = i; primes.push_back(i); }
            for (int p : primes) {
                if (p > spf[i] || 1LL * p * i > n) break;
                spf[p * i] = p;
            }
        }
    }

    bool is_prime(int x) const { return x >= 2 && spf[x] == x; }

    // Devuelve {(primo, exponente)}.
    std::vector<std::pair<int, int>> factor(int x) const {
        std::vector<std::pair<int, int>> res;
        while (x > 1) {
            int p = spf[x], c = 0;
            while (x % p == 0) x /= p, ++c;
            res.emplace_back(p, c);
        }
        return res;
    }

    std::vector<int> divisors(int x) const {
        std::vector<int> d{1};
        for (auto [p, c] : factor(x)) {
            int cur = (int)d.size(), pw = 1;
            for (int e = 1; e <= c; ++e) {
                pw *= p;
                for (int i = 0; i < cur; ++i) d.push_back(d[i] * pw);
            }
        }
        std::sort(d.begin(), d.end());
        return d;
    }
};
