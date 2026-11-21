import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Leer los puntos del archivo CSV
data = pd.read_csv(input("Archivo de coordenadas (CSV): "))

points = data[data["type"] == "point"][["x", "y"]].to_numpy()
hull = data[data["type"] == "hull"][["x", "y"]].to_numpy()

# Graficar los puntos
plt.scatter(points[:, 0], points[:, 1], color='blue', label='Points')

# Cubrir los puntos con el hull
hull_closed = np.vstack([hull, hull[0]])

plt.plot(hull_closed[:, 0], hull_closed[:, 1], color='red', linewidth=2, label='Convex Hull')

# Formato del gráfico
plt.xlabel('X')
plt.ylabel('Y')
plt.title('Convex Hull Visualization')
plt.legend()
plt.grid(True)
plt.axis('equal')

plt.show()