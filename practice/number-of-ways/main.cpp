/**
 *  Autor : JaimeAntonioRP
 *  Problema:
 *  Enlace  :
 *  Idea    :
 */
#include <bits/stdc++.h>
using namespace std;

using ll  = long long;
using ull = unsigned long long;
using ld  = long double;
using pii = pair<int, int>;
using pll = pair<ll, ll>;
template <class T> using vc  = vector<T>;
template <class T> using vvc = vector<vector<T>>;
template <class T> using minpq = priority_queue<T, vector<T>, greater<T>>;

#define all(x)  (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#define sz(x)   ((int)(x).size())
#define pb      push_back
#define eb      emplace_back
#define fi      first
#define se      second

#ifdef LOCAL
#include "debug.hpp"
#else
#define dbg(...)  ((void)0)
#define dbgr(...) ((void)0)   // sin esto, un dbgr() olvidado = CE en el juez
#endif

constexpr int INF = 1'000'000'007;
constexpr ll  LNF = 4'000'000'000'000'000'000LL;

void solve() {
    int n; cin >> n;
    vc<ll> ways(n);
    ll s = 0;
    for (int i = 0; i < n; ++i) {
        cin >> ways[i];
        s += ways[i];
    }
    dbg(ways);
    dbg(s, n);
    if(s % 3 != 0 || n < 3) {
        cout << 0 << "\n";
        return;
    }
    ll curr =  s / 3;
    ll ans = 0;
    ll s1 = 0;
    ll s13 = 0;
    for (int i = 0; i < n - 1; ++i){
        s1 += ways[i];
        if(s1 == 2 * curr) {
            ans += s13;
        }
        if(s1 == curr) s13++;
    }
    cout << ans << "\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T = 1;
    // cin >> T;                       // <- descomenta si hay multiples casos
    for (int tc = 1; tc <= T; ++tc) {
        // cout << "Case #" << tc << ": ";
        solve();
    }
    return 0;
}
/*

11
0 1 3 -3 3  0 4 2 2 0 0
1 3 -3 3 0 4 2 2  


it 1 =



*/

