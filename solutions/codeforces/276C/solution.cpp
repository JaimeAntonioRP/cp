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
    int n, q; cin >> n >> q;
    vc<ll> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    sort(all(a));
    vc<ll> prefix(n + 1, 0);
    while(q--){
        int l, r; cin >> l >> r;
        l--, r--;
        prefix[l]++;
        prefix[r + 1]--;
    }
    
    for(int i = 0; i < n; i++){
        prefix[i + 1] += prefix[i];
    }
    sort(all(prefix));
    ll ans = 0;
    for(int i = 0; i < n; i++){
        ans += prefix[i + 1] * a[i];
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
