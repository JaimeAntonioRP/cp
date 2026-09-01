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
    // <<< tu solucion aqui >>>    
    int n; cin >> n;
    vc<int> a(n);
    for (int i = 0; i < n; ++i) cin >> a[i];
    int ans = 0;
    int ub = 0;
    dbg(a);
    for(int i = 0; i < n; i++){
        if (i == 0 || i == n - 1){
            if(i == 0) {
                if(abs(a[i] - a[i + 1]) > ans) {
                    ans = abs(a[i] - a[i + 1]);
                    ub = 0;
                }
                
            }
            else {
                if(abs(a[i] - a[i - 1]) > ans) {
                    ans = abs(a[i] - a[i - 1]);
                    ub = i;
                }
            }
        }
        else{
            if(abs(a[i] - a[i+ 1]) + abs(a[i] - a[i - 1]) > ans) {
                ans = abs(a[i] - a[i+ 1]) + abs(a[i] - a[i - 1]);
                ub = i;
            }
        }
    }
    ll s = 0;
    for(int i = 0; i < n - 1; i++){
        if(i == ub) continue;
        if(i + 1 == ub) continue;
        s += abs(a[i] - a[i + 1]);
    }
    dbg(ub);
    
    cout << s << "\n";

}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T = 1;
    cin >> T;                       // <- descomenta si hay multiples casos
    for (int tc = 1; tc <= T; ++tc) {
        // cout << "Case #" << tc << ": ";
        solve();
    }
    return 0;
}
