/**
 *  debug.hpp — impresion generica para depurar en local.
 *  Solo se incluye cuando se compila con -DLOCAL (ver templates/main.cpp).
 *
 *  Uso:   dbg(n, a, mp);   ->   [12] n, a, mp = 5 | {1, 2, 3} | {(1, "x")}
 *  Todo sale por stderr, asi que jamas ensucia la salida enviada al juez.
 */
#pragma once
#include <bits/stdc++.h>

namespace dbg_impl {
using namespace std;

void print(const string& x);
void print(const char* x);
void print(char x);
void print(bool x);
template <class A, class B> void print(const pair<A, B>& x);
template <class T, class = decltype(begin(declval<const T&>()))> void print(const T& x);
template <class T, class = typename enable_if<is_arithmetic<T>::value>::type> void print(T x);

inline void print(const string& x) { cerr << '"' << x << '"'; }
inline void print(const char* x)   { cerr << '"' << x << '"'; }
inline void print(char x)          { cerr << '\'' << x << '\''; }
inline void print(bool x)          { cerr << (x ? "true" : "false"); }

template <class A, class B> void print(const pair<A, B>& x) {
    cerr << '(';
    print(x.first);
    cerr << ", ";
    print(x.second);
    cerr << ')';
}

template <class T, class> void print(const T& x) {
    cerr << '{';
    bool first = true;
    for (const auto& e : x) {
        if (!first) cerr << ", ";
        first = false;
        print(e);
    }
    cerr << '}';
}

template <class T, class> void print(T x) { cerr << x; }

inline void print_all() {}
template <class T, class... R> void print_all(const T& x, const R&... rest) {
    print(x);
    if (sizeof...(rest)) cerr << " | ";
    print_all(rest...);
}
}  // namespace dbg_impl

// Sin secuencias ANSI: la consola de Windows no las interpreta por defecto y
// saldrian como basura tipo "[36m" en medio de la salida de `cpx test`.
#define dbg(...)                                                        \
    do {                                                                \
        std::cerr << "[L" << __LINE__ << "] " << #__VA_ARGS__ << " = "; \
        dbg_impl::print_all(__VA_ARGS__);                               \
        std::cerr << std::endl;                                         \
    } while (0)
