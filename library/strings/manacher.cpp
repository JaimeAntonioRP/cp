// Manacher: para cada centro, el radio del palindromo mas largo.  O(n).
// d1[i] = # de palindromos impares centrados en i   (longitud 2*d1[i]-1)
// d2[i] = # de palindromos pares centrados entre i-1 e i (longitud 2*d2[i])
#pragma once
#include <bits/stdc++.h>

std::pair<std::vector<int>, std::vector<int>> manacher(const std::string& s) {
    int n = (int)s.size();
    std::vector<int> d1(n), d2(n);

    for (int i = 0, l = 0, r = -1; i < n; ++i) {
        int k = (i > r) ? 1 : std::min(d1[l + r - i], r - i + 1);
        while (i - k >= 0 && i + k < n && s[i - k] == s[i + k]) ++k;
        d1[i] = k--;
        if (i + k > r) l = i - k, r = i + k;
    }
    for (int i = 0, l = 0, r = -1; i < n; ++i) {
        int k = (i > r) ? 0 : std::min(d2[l + r - i + 1], r - i + 1);
        while (i - k - 1 >= 0 && i + k < n && s[i - k - 1] == s[i + k]) ++k;
        d2[i] = k--;
        if (i + k > r) l = i - k - 1, r = i + k;
    }
    return {d1, d2};
}

// Longitud del palindromo mas largo.
inline int longest_palindrome(const std::string& s) {
    auto [d1, d2] = manacher(s);
    int best = 0;
    for (int x : d1) best = std::max(best, 2 * x - 1);
    for (int x : d2) best = std::max(best, 2 * x);
    return best;
}
