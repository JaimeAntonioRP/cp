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

string max(int n, int m){
    int nines = m / 9;
    int rem = m % 9;
    string ans = "";
    for(int i = 0; i < nines; i++){
        ans += '9';
    }
    if(rem > 0){
        ans += (char)(rem + '0');
    }
    for(int i = nines + (rem > 0 ? 1 : 0); i < n; i++){
        ans += '0';
    }
    return ans;
}

string min(int n, int m){
    string ans = "";
    for(int i = 0; i < n; i++){
        if(9 * (n - i - 1) >= m){
            if(i == 0 ){
                ans += '1';
                m -= 1;
            } else {
                ans += '0';
            }
        } else {
            int d = m - 9 * (n - i - 1);
            ans += (char)(d + '0');
            m -= d;
        }
    }
    return ans;
}
void solve() {
    // <<< tu solucion aqui >>>   
    int n, m; cin >> n >> m;
    
    if(n == 1 && m == 0){
        cout << "0 0\n";
        return; 
    }

    if(m == 0 ){
        cout << "-1 -1\n";
        return; 
    } 

    if(m > 9*n){
        cout << "-1 -1\n";
        return; 
    }
    cout << min(n, m) << " " << max(n, m) << "\n";
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
max 
m = 13
n = 2



*/
