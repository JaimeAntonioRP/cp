// Prefix function (KMP).  pi[i] = mayor borde propio de s[0..i].  O(n).
#pragma once
#include <bits/stdc++.h>

std::vector<int> prefix_function(const std::string& s) {
    int n = (int)s.size();
    std::vector<int> pi(n, 0);
    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j]) j = pi[j - 1];
        if (s[i] == s[j]) ++j;
        pi[i] = j;
    }
    return pi;
}

// Posiciones (0-based) donde aparece `pat` dentro de `txt`.
std::vector<int> kmp_search(const std::string& pat, const std::string& txt) {
    std::string s = pat + '\x01' + txt;
    std::vector<int> pi = prefix_function(s), res;
    int m = (int)pat.size();
    for (int i = m + 1; i < (int)s.size(); ++i)
        if (pi[i] == m) res.push_back(i - 2 * m);
    return res;
}

// Periodo minimo de s (si s no es periodica, devuelve |s|).
inline int min_period(const std::string& s) {
    int n = (int)s.size();
    std::vector<int> pi = prefix_function(s);
    int p = n - pi[n - 1];
    return n % p == 0 ? p : n;
}
