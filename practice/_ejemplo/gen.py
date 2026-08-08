"""Generador del ejemplo: arreglos cortos con negativos, que es donde suelen
   esconderse los bugs (inicializar `best` a 0 en vez de a a[0], por ejemplo)."""
import random
import sys

rnd = random.Random(int(sys.argv[1]) if len(sys.argv) > 1 else 0)

n = rnd.randint(1, 6)
print(n)
print(*[rnd.randint(-5, 5) for _ in range(n)])
