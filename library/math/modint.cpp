// Aritmetica modular estatica.  Todas las operaciones en O(1) salvo inv/pow: O(log MOD).
//   using mint = ModInt<998244353>;   (o 1'000'000'007)
#pragma once
#include <bits/stdc++.h>

template <int MOD>
struct ModInt {
    int v;

    ModInt() : v(0) {}
    ModInt(long long x) : v(int(x % MOD)) { if (v < 0) v += MOD; }

    static constexpr int mod() { return MOD; }
    explicit operator int() const { return v; }

    ModInt& operator+=(const ModInt& o) { if ((v += o.v) >= MOD) v -= MOD; return *this; }
    ModInt& operator-=(const ModInt& o) { if ((v -= o.v) < 0) v += MOD; return *this; }
    ModInt& operator*=(const ModInt& o) { v = int(1LL * v * o.v % MOD); return *this; }
    ModInt& operator/=(const ModInt& o) { return *this *= o.inv(); }

    friend ModInt operator+(ModInt a, const ModInt& b) { return a += b; }
    friend ModInt operator-(ModInt a, const ModInt& b) { return a -= b; }
    friend ModInt operator*(ModInt a, const ModInt& b) { return a *= b; }
    friend ModInt operator/(ModInt a, const ModInt& b) { return a /= b; }
    ModInt operator-() const { return ModInt(0) - *this; }

    bool operator==(const ModInt& o) const { return v == o.v; }
    bool operator!=(const ModInt& o) const { return v != o.v; }

    ModInt pow(long long e) const {
        ModInt r(1), b(*this);
        for (; e > 0; e >>= 1, b *= b)
            if (e & 1) r *= b;
        return r;
    }
    ModInt inv() const { return pow(MOD - 2); }   // requiere MOD primo

    friend std::ostream& operator<<(std::ostream& os, const ModInt& x) { return os << x.v; }
    friend std::istream& operator>>(std::istream& is, ModInt& x) {
        long long t; is >> t; x = ModInt(t); return is;
    }
};

using mint = ModInt<998244353>;
// using mint = ModInt<1'000'000'007>;
