// Z-function: z[i] = longitud del prefijo comun mas largo entre s y s[i..].  O(n).
// Busqueda de patron: z_function(pat + '\x01' + txt), buscar z[i] == |pat|.
#pragma once
#include <bits/stdc++.h>

std::vector<int> z_function(const std::string& s) {
    int n = (int)s.size();
    std::vector<int> z(n, 0);
    for (int i = 1, l = 0, r = 0; i < n; ++i) {
        if (i < r) z[i] = std::min(r - i, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) ++z[i];
        if (i + z[i] > r) l = i, r = i + z[i];
    }
    if (n) z[0] = n;
    return z;
}
