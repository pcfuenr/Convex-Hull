import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Leer los puntos del archivo CSV
data = pd.read_csv(input("Archivo de coordenadas (CSV): "))

points = data[data["type"] == "point"][["x", "y"]].to_numpy()
hull = data[data["type"] == "hull"][["x", "y"]].to_numpy()

# Ordenar los puntos del hull para que se conecten correctamente
center = np.mean(hull, axis=0)
angles = np.arctan2(hull[:, 1] - center[1], hull[:, 0] - center[0])
hull = hull[np.argsort(angles)]

# Graficar los puntos
# Si hay muchos puntos, se puede modificar el valor de 's' para cambiar el tamaño de los puntos
plt.scatter(points[:, 0], points[:, 1], color='blue', s=10)

# Cubrir los puntos con el hull
hull_closed = np.vstack([hull, hull[0]])

plt.plot(hull_closed[:, 0], hull_closed[:, 1], color='red', linewidth=1)

# Formato del gráfico
plt.xlabel('X')
plt.ylabel('Y')
plt.title('Visualización del Convex Hull')
plt.grid(True)
plt.axis('equal')

plt.show()