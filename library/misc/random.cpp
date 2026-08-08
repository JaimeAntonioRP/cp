// RNG con semilla no predecible + hash anti-colision para unordered_map/set.
// Sin esto, un test adversario puede volver tu unordered_map O(n) por operacion.
#pragma once
#include <bits/stdc++.h>

static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());

long long rnd(long long l, long long r) {   // entero uniforme en [l, r]
    return std::uniform_int_distribution<long long>(l, r)(rng);
}

struct custom_hash {
    static uint64_t splitmix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }
    size_t operator()(uint64_t x) const {
        static const uint64_t FIXED = std::chrono::steady_clock::now().time_since_epoch().count();
        return (size_t)splitmix64(x + FIXED);
    }
};

// Uso:  std::unordered_map<long long, int, custom_hash> mp;
//       mp.reserve(1 << 20);  mp.max_load_factor(0.25);
