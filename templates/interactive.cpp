/**
 *  Plantilla para problemas INTERACTIVOS.
 *  Regla de oro: NUNCA uses cin.tie(nullptr) sin flush explicito.
 */
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

// Pregunta al juez y devuelve su respuesta.
ll query(int x) {
    cout << "? " << x << endl;   // endl == '\n' + flush (imprescindible)
    ll res;
    cin >> res;
    if (res == -1) exit(0);      // el juez aborto: salir limpio
    return res;
}

void answer(ll x) {
    cout << "! " << x << endl;
}

void solve() {
    int n;
    cin >> n;
    // <<< tu interaccion aqui >>>
}

int main() {
    // sin sync_with_stdio(false) para evitar sorpresas de buffering
    int T = 1;
    cin >> T;
    while (T--) solve();
    return 0;
}
