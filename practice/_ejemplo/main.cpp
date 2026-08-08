/**
 *  Autor : JaimeAntonioRP
 *  Problema: EJEMPLO — maximo de un arreglo.
 *  Enlace  : (ninguno, es la demo del entorno)
 *  Idea    : recorrer y quedarse con el mayor.
 *
 *  Sirve de plantilla viva para ver el flujo completo:
 *      cpx test practice/_ejemplo      -> 2/2 casos correctos
 *      cpx stress practice/_ejemplo    -> compara contra brute.cpp con gen.py
 */
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
#define all(x) (x).begin(), (x).end()
#define sz(x)  ((int)(x).size())

#ifdef LOCAL
#include "debug.hpp"
#else
#define dbg(...) ((void)0)
#endif

void solve() {
    int n;
    cin >> n;
    vector<int> a(n);
    for (auto& x : a) cin >> x;
    dbg(n, a);                       // solo se ve en local, nunca en el juez
    cout << *max_element(all(a)) << '\n';
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int T = 1;
    for (int tc = 1; tc <= T; ++tc) solve();
    return 0;
}
