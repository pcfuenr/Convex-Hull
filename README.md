# PROYECTO CONVEX HULL EDAA

En este repositorio se investigará, implementará, evaluará y comparará 3 algoritmos para
resolver el problema de Convex Hull, estos algoritmos serán:
- Jarvis March
- Divide & Conquer
- Graham Scan

## Para ejecutar
Se compila el main con

```
g++ main.cpp -o main
```

Y se ejecuta con

```
.\main <coordenadas_resultantes>.csv
```

Donde `<coordenadas_resultantes>.csv` corresponde a un archivo CSV que guarda las coordenadas de los puntos y hull resultantes del algoritmo para luego graficarlos.

## Para graficar
Se ejecuta el programa de python con

```
python visual.py
```

Que desplegará un input para ingresar la ruta donde se encuentra el archivo con las coordenadas a graficar.

## Para experimentar
El script de experimentación es ejecutable en Windows, para su correcto funcionamiento es necesario que `/datasets/` contenga grafos.

```
.\run_experiments.ps1
```
Compilará `uhr.cpp` y experimentará los datasets que encuentre en el directorio mencionado anteriormente, finalmente, exporta todos los resultados en formato `.csv` al directorio `/resultados/`.

## Para demostración paso a paso:

Aparecera 5 iteraciones del algoritmo divide and conquer de manera visual, con 2 botones para retroceder o avanzar en la iteración

python visual.py demo_incremental.csv