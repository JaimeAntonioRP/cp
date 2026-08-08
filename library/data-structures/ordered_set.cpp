// Ordered set de GNU pb_ds: set con order_of_key / find_by_order en O(log n).
// Para multiset usa pares (valor, contador_unico) o cambia less<T> por less_equal<T>
// (ojo: con less_equal, erase(valor) deja de funcionar; borra por iterador).
#pragma once
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

template <class T>
using ordered_set = __gnu_pbds::tree<T, __gnu_pbds::null_type, std::less<T>,
                                     __gnu_pbds::rb_tree_tag,
                                     __gnu_pbds::tree_order_statistics_node_update>;

template <class K, class V>
using ordered_map = __gnu_pbds::tree<K, V, std::less<K>,
                                     __gnu_pbds::rb_tree_tag,
                                     __gnu_pbds::tree_order_statistics_node_update>;

//  s.order_of_key(x)  -> cuantos elementos son < x
//  *s.find_by_order(k) -> el k-esimo elemento (0-based)
