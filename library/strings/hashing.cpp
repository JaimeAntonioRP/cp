// Hashing doble de cadenas con base aleatoria (resistente a tests anti-hash).
// Construccion O(n), hash de substring O(1).
#pragma once
#include <bits/stdc++.h>

struct StrHash {
    using ll = long long;
    static constexpr ll M1 = 1'000'000'007, M2 = 998'244'353;
    static inline ll B1 = 0, B2 = 0;

    std::vector<ll> h1, h2, p1, p2;

    static void init_bases() {
        if (B1) return;
        std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        B1 = 131 + (ll)(rng() % 1000) * 2;
        B2 = 137 + (ll)(rng() % 1000) * 2;
    }

    explicit StrHash(const std::string& s) {
        init_bases();
        int n = (int)s.size();
        h1.assign(n + 1, 0); h2.assign(n + 1, 0);
        p1.assign(n + 1, 1); p2.assign(n + 1, 1);
        for (int i = 0; i < n; ++i) {
            h1[i + 1] = (h1[i] * B1 + s[i]) % M1;
            h2[i + 1] = (h2[i] * B2 + s[i]) % M2;
            p1[i + 1] = p1[i] * B1 % M1;
            p2[i + 1] = p2[i] * B2 % M2;
        }
    }

    // Hash de s[l, r)  — comparalo con == entre substrings.
    std::pair<ll, ll> get(int l, int r) const {
        ll a = (h1[r] - h1[l] * p1[r - l]) % M1;
        ll b = (h2[r] - h2[l] * p2[r - l]) % M2;
        if (a < 0) a += M1;
        if (b < 0) b += M2;
        return {a, b};
    }
};
