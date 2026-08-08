// Utilidades de teoria de numeros: exponenciacion, gcd extendido, inverso, CRT,
// Miller-Rabin determinista y Pollard's rho (factoriza hasta 10^18).
#pragma once
#include <bits/stdc++.h>

using ll  = long long;
using ull = unsigned long long;

ll power_mod(ll b, ll e, ll m) {
    ll r = 1;
    b %= m;
    if (b < 0) b += m;
    // el % m garantiza que el resultado cabe en ll; el cast lo hace explicito
    for (; e > 0; e >>= 1, b = (ll)((__int128)b * b % m))
        if (e & 1) r = (ll)((__int128)r * b % m);
    return r;
}

// Devuelve g = gcd(a, b) >= 0 y x, y tales que a*x + b*y = g.
// El caso base normaliza el signo: con a < 0 la version ingenua devuelve g < 0,
// lo que rompe silenciosamente a inv_mod y crt.
ll extgcd(ll a, ll b, ll& x, ll& y) {
    if (b == 0) {
        x = (a < 0) ? -1 : 1;
        y = 0;
        return std::abs(a);
    }
    ll x1, y1, g = extgcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

// Inverso de a mod m (m no necesariamente primo).  -1 si no existe.
ll inv_mod(ll a, ll m) {
    ll x, y;
    if (extgcd(a, m, x, y) != 1) return -1;
    return ((x % m) + m) % m;
}

// x = r1 (mod m1), x = r2 (mod m2).  Devuelve {resto, modulo} o {-1,-1} si no hay solucion.
std::pair<ll, ll> crt(ll r1, ll m1, ll r2, ll m2) {
    ll x, y, g = extgcd(m1, m2, x, y);
    if ((r2 - r1) % g != 0) return {-1, -1};

    ll lcm  = m1 / g * m2;
    ll mod2 = m2 / g;
    ll t = (ll)((__int128)((r2 - r1) / g) % mod2 * x % mod2);
    if (t < 0) t += mod2;                       // t debe caer en [0, m2/g)
    ll res = (ll)(((__int128)t * m1 + r1) % lcm);
    if (res < 0) res += lcm;
    return {res, lcm};
}

bool is_prime(ll n) {
    if (n < 2) return false;
    for (ll p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
        if (n % p == 0) return n == p;
    }
    ll d = n - 1;
    int s = 0;
    while (!(d & 1)) d >>= 1, ++s;
    for (ll a : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
        ll x = power_mod(a, d, n);
        if (x == 1 || x == n - 1) continue;
        bool comp = true;
        for (int i = 1; i < s; ++i) {
            x = (ll)((__int128)x * x % n);
            if (x == n - 1) { comp = false; break; }
        }
        if (comp) return false;
    }
    return true;
}

ll pollard(ll n) {
    if (n % 2 == 0) return 2;
    for (ll c = 1;; ++c) {
        auto f = [&](ll x) -> ll { return (ll)(((__int128)x * x + c) % n); };
        ll x = 2, y = 2, d = 1;
        while (d == 1) {
            x = f(x);
            y = f(f(y));
            d = std::__gcd(std::abs(x - y), n);
        }
        if (d != n) return d;
    }
}

void factor_rec(ll n, std::map<ll, int>& res) {
    if (n == 1) return;
    if (is_prime(n)) { ++res[n]; return; }
    ll d = pollard(n);
    factor_rec(d, res);
    factor_rec(n / d, res);
}

std::map<ll, int> factorize(ll n) {
    std::map<ll, int> res;
    factor_rec(n, res);
    return res;
}
