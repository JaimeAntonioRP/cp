# cp — programación competitiva en C++

Repositorio de trabajo: plantillas, librería de estructuras, gestión de concursos,
scripts de compilación/testing y archivo automático de soluciones aceptadas.

```
cp/
├── cpx.cmd / cpx            <- CLI (llámalo desde cualquier carpeta del repo)
├── config.json              <- compilador, flags, timeout, juez por defecto
├── templates/               <- main.cpp, brute.cpp, gen.py, interactive.cpp
├── library/                 <- estructuras y algoritmos reutilizables
│   ├── data-structures/     dsu, fenwick, segtree, lazy, sparse table, ordered set
│   ├── graphs/              dijkstra, bfs01, lca, scc, toposort, kruskal
│   ├── math/                modint, comb, sieve, teoría de números, matrix
│   ├── strings/             z-function, kmp, hashing, manacher
│   └── misc/                debug.hpp, random, fastio
├── contests/                <- concursos en curso (uno por carpeta con fecha)
├── practice/                <- problemas sueltos
├── solutions/               <- ARCHIVO de aceptados + README.md con el índice
├── scripts/                 <- implementación del CLI
└── .vscode/                 <- tareas (Ctrl+Shift+B) y snippets
```

---

## Puesta a punto

### 1. Compilador — ya instalado

Está instalado **WinLibs GCC 16.1.0** (MinGW-w64, UCRT) vía winget, y winget ya lo
puso en el `PATH` del usuario. Si abres una terminal nueva:

```bash
cpx doctor
```

debe mostrar la ruta de `g++`. Para reinstalarlo o actualizarlo en el futuro:

```bash
winget install -e --id BrechtSanders.WinLibs.POSIX.UCRT
```

### 2. Poner `cpx` en el PATH (opcional pero recomendado)

Para poder escribir `cpx test` desde cualquier subcarpeta:

```powershell
[Environment]::SetEnvironmentVariable('PATH', $env:PATH + ';D:\cp\cp', 'User')
```

Reinicia la terminal. Sin esto, usa la ruta completa: `D:\cp\cp\cpx.cmd test`.

---

## Flujo de trabajo

### Un problema suelto

```bash
cpx new two-sum -Tests 2 -Open
```

Crea `practice/two-sum/` con `main.cpp` y `tests/1.in`, `tests/1.out`, `2.in`, `2.out`.
Pegas los ejemplos del enunciado en los `.in`/`.out`, escribes la solución y:

```bash
cpx test practice/two-sum
```

Salida típica:

```
  [1     ] OK      12 ms
  [2     ] WA      linea 3  (9 ms)
           esperado : 4 7
           obtenido : 4 8

  1/2 casos correctos
```

### Un concurso

```bash
cpx contest cf-round-999 -Problems A-F -Judge codeforces -Url https://codeforces.com/contest/1999
```

Crea `contests/2026-08-08-cf-round-999/` con las carpetas `A/` … `F/`, cada una con
plantilla y tests vacíos, más un `contest.json` y un `README.md` con checklist.

Durante el concurso, dentro de la carpeta del problema (sin argumentos usa la carpeta actual):

```bash
cpx test          # compila y corre todos los casos
cpx tc            # añade tests/2.in y tests/2.out y los abre
cpx run -In tests/1.in
cpx run           # ejecución interactiva, tecleas la entrada a mano
```

### Cuando el juez te da Accepted

```bash
cpx accept -Id 1999A -Name "Bipartite Array" -Tags "greedy,dp"
```

Esto:

1. copia el código a `solutions/codeforces/1999A/solution.cpp` junto con los tests y un `meta.json`,
2. regenera `solutions/README.md` (tabla con fecha, juez, id, tags y enlace al código),
3. hace `git add` + `git commit -m "solve(codeforces): 1999A - Bipartite Array"`,
4. hace `git push` a GitHub.

Con `-NoPush` se queda solo en local. Con `-Message "..."` cambias el mensaje del commit.

Para subir trabajo a medias (sin archivar nada):

```bash
cpx sync -Message "wip: intentos ronda 999"
```

---

## Depuración

`main.cpp` incluye `debug.hpp` solo cuando se compila con `-DLOCAL`, que es lo que hace
`cpx` por defecto. Así que puedes dejar los `dbg()` puestos y el juez nunca los verá:

```cpp
dbg(n, a, mp);   // -> [23] n, a, mp = 5 | {1, 2, 3} | {(1, "x")}
```

Todo sale por `stderr`, nunca contamina la respuesta.

El modo por defecto compila con `-O0 -g -Wall -Wextra -Wshadow -Wconversion -D_GLIBCXX_DEBUG`.
Ese último detecta accesos fuera de rango en `vector`, `string`, etc. **en tiempo de ejecución**,
que es de lejos la fuente más común de RE misteriosos. Y no es teoría: te dice exactamente

```
Error: attempt to subscript container with out-of-bounds index 10,
but container only holds 3 elements.
```

Cuando algo sigue sin cuadrar:

```bash
cpx test -San        # sanitizers
cpx test -Release    # -O2, para medir tiempos reales
```

> **Nota sobre `-San` en Windows.** MinGW-w64 no distribuye `libasan`/`libubsan`, así que
> AddressSanitizer no está disponible. `cpx` lo detecta y reintenta automáticamente con
> `-fsanitize=undefined -fsanitize-trap=undefined`, que sí funciona sin runtime: el
> comportamiento indefinido (overflow con signo, shifts inválidos…) aborta el programa con
> *Illegal instruction*, pero sin mensaje descriptivo. En la práctica `-D_GLIBCXX_DEBUG`,
> que ya está activo por defecto, cubre la mayoría de los bugs de programación competitiva.

## Autotest de la librería

```bash
cpx run library/selftest.cpp
```

Contrasta cada estructura contra fuerza bruta con datos aleatorios y debe imprimir
`todo correcto`. Ejecútalo **cada vez que añadas o toques un snippet** — ya sirvió para
cazar un `extgcd` que devolvía `gcd` negativo con entradas negativas, lo que rompía en
silencio a `inv_mod` y a `crt`.

## Stress testing (encontrar el contraejemplo)

Cuando tienes WA en un test grande y oculto:

```bash
cpx stress
```

La primera vez copia `brute.cpp` y `gen.py` a la carpeta del problema para que los
completes: `brute.cpp` es la versión lenta pero obviamente correcta, y `gen.py` genera
casos aleatorios **pequeños**. Después, `cpx stress -Iters 500` compara ambas salidas
hasta encontrar una discrepancia — y cuando la encuentra, la imprime y la guarda
automáticamente como un nuevo caso en `tests/`.

## Librería

```bash
cpx lib              # lista todo, agrupado por categoría
cpx lib dijkstra     # busca por nombre y por contenido
```

Los snippets son ficheros con `#pragma once`, así que puedes incluirlos directamente
(el `-I library` lo añade `cpx` al compilar):

```cpp
#include "data-structures/dsu.cpp"
#include "math/modint.cpp"
```

Para enviar al juez, pega el contenido del snippet en el fichero (los jueces solo
aceptan un archivo). El `#include` local es para probar rápido y para stress tests.

## Progreso

```bash
cpx stats
```

---

## Referencia de comandos

| Comando | Qué hace |
| --- | --- |
| `cpx new <nombre>` | crea `practice/<nombre>/` con plantilla y tests |
| `cpx contest <slug>` | crea `contests/<fecha>-<slug>/` con N problemas |
| `cpx tc [ruta]` | añade el siguiente par `n.in` / `n.out` |
| `cpx run [ruta]` | compila y ejecuta (`-In fichero` para redirigir la entrada) |
| `cpx test [ruta]` | compila y corre todos los casos, comparando salidas |
| `cpx stress [ruta]` | busca contraejemplos con `brute.cpp` + `gen.py` |
| `cpx accept [ruta]` | archiva en `solutions/`, commit y push |
| `cpx sync` | commit + push de todo lo pendiente |
| `cpx lib [query]` | lista o busca en la librería |
| `cpx stats` | resueltos por juez, por tag y de los últimos 7 días |
| `cpx doctor` | comprueba compilador, python, git y remoto |

Todos los comandos aceptan la ruta del problema como primer argumento; si la omites
se usa la carpeta actual. Atajos: `n`, `c`, `r`, `t`, `s`, `a`, `l`.
