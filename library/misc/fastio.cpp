// Lector rapido de enteros por buffer.  Util cuando n >= 10^6 y cin va justo.
// Solo lee enteros con signo y saltos de espacio; no maneja floats ni strings.
#pragma once
#include <bits/stdc++.h>

namespace fio {
static char buf[1 << 22];
static size_t len = 0, pos = 0;

inline char gc() {
    if (pos == len) {
        len = fread(buf, 1, sizeof(buf), stdin);
        pos = 0;
        if (len == 0) return EOF;
    }
    return buf[pos++];
}

template <class T>
inline T read() {
    char c = gc();
    while (c != '-' && (c < '0' || c > '9')) c = gc();
    bool neg = (c == '-');
    if (neg) c = gc();
    T x = 0;
    while (c >= '0' && c <= '9') x = x * 10 + (c - '0'), c = gc();
    return neg ? -x : x;
}
}  // namespace fio

// Uso:  int n = fio::read<int>();   long long x = fio::read<long long>();
// Para escribir rapido basta con '\n' en vez de endl y cout con sync desactivado.
