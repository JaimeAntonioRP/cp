/**
 *  Fuerza bruta del ejemplo: O(n^2) en vez de O(n), pero imposible de equivocar.
 *  Asi es como debe verse un brute.cpp: lento y obviamente correcto.
 */
#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    cin >> n;
    vector<int> a(n);
    for (auto& x : a) cin >> x;

    int best = a[0];
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) best = max(best, max(a[i], a[j]));

    cout << best << '\n';
    return 0;
}
