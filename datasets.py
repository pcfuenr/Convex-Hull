import os
import math
import random

# fijamos la semilla para repetir siempre los mismos experimentos
random.seed(42)

# carpeta donde van a quedar los archivos
carpeta = "datasets"
if not os.path.exists(carpeta):
    os.makedirs(carpeta)


def guardar(nombre, puntos):
    ruta = os.path.join(carpeta, nombre)
    with open(ruta, "w") as f:
        # primera linea con la cantidad total de puntos
        f.write(f"{len(puntos)}\n")
        for x, y in puntos:
            f.write(f"{x:.6f} {y:.6f}\n")
    print(f"Listo: {ruta}")


# 1. puntos repartidos al azar en un cuadrado (caso promedio)
def crear_uniforme(n):
    puntos = []
    for _ in range(n):
        x = random.uniform(0, 10000)
        y = random.uniform(0, 10000)
        puntos.append((x, y))
    return puntos


# 2. nube densa al centro y pocos puntos afuera (hull muy chico)
def crear_gaussiano(n):
    puntos = []
    for _ in range(n):
        x = random.gauss(0, 1000)
        y = random.gauss(0, 1000)
        puntos.append((x, y))
    return puntos


# 3. todos los puntos en el borde de un circulo (peor caso para jarvis march)
def crear_circulo(n):
    puntos = []
    radio = 5000
    for i in range(n):
        angulo = (2 * math.pi * i) / n
        x = radio * math.cos(angulo)
        y = radio * math.sin(angulo)
        puntos.append((x, y))
    # los desordenamos para no darle ventaja a los algoritmos que ordenan
    random.shuffle(puntos)
    return puntos


# 4. cuadricula entera
def crear_grilla(n):
    lado = int(math.isqrt(n))
    puntos = []
    for i in range(lado):
        for j in range(lado):
            puntos.append((float(i), float(j)))
    # rellenamos adentro por si n no tiene raiz exacta
    while len(puntos) < n:
        x = random.uniform(0, lado - 1)
        y = random.uniform(0, lado - 1)
        puntos.append((x, y))
    random.shuffle(puntos)
    return puntos


# 5. varios grupos separados en distintas partes del plano
def crear_clusters(n):
    centros = [
        (1000, 1000),
        (8000, 2000),
        (5000, 5000),
        (2000, 8500),
        (9000, 9000),
    ]
    puntos = []
    for _ in range(n):
        cx, cy = random.choice(centros)
        x = random.gauss(cx, 300)
        y = random.gauss(cy, 300)
        puntos.append((x, y))
    return puntos


# varios tamaños
tamanos = [10, 100, 1000, 10000, 100000, 100000]

for n in tamanos:
    guardar(f"uniforme_{n}.txt", crear_uniforme(n))
    guardar(f"gaussiano_{n}.txt", crear_gaussiano(n))
    guardar(f"circulo_{n}.txt", crear_circulo(n))
    guardar(f"grilla_{n}.txt", crear_grilla(n))
    guardar(f"clusters_{n}.txt", crear_clusters(n))