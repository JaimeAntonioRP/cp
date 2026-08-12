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

bool flag1(string s, string s1, string s2){
    bool ans = false;
    int l = 0;
    int n = sz(s);
    while(l < n - 3){
        string aux = s.substr(l, 2);
        dbg(l, aux);
        if(aux == s1){
            int r = l + 2;
            dbg(r, s.substr(r, 2));
            while(r < n - 1){
                string aux2 = s.substr(r, 2);
                dbg(r, aux2);
                if(aux2 == s2){
                    ans = true;
                    break;
                }
                r++;
            }
            break;
        }
        l++;
    }
    return ans;
}

void solve() {
    string s; cin >> s;
    bool f = flag1(s, "BA", "AB") || flag1(s, "AB", "BA");
    cout << (f ? "YES" : "NO") << "\n";
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
