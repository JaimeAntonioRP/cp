"""
Generador de tests aleatorios para `cpx stress`.

Se invoca como:  python gen.py <semilla>
y debe imprimir en stdout UN caso de prueba valido, con parametros PEQUENOS
(n <= 8, valores <= 10) para que la fuerza bruta termine y el contraejemplo
sea legible a simple vista.
"""
import random
import sys

seed = int(sys.argv[1]) if len(sys.argv) > 1 else 0
rnd = random.Random(seed)

# ---------------------------------------------------------------- parametros
MAXN = 8
MAXV = 10

# ------------------------------------------------------------------- ejemplo
n = rnd.randint(1, MAXN)
a = [rnd.randint(1, MAXV) for _ in range(n)]

print(n)
print(*a)
