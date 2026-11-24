#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "d&c.cpp"

std::vector<std::vector<double>> loadDataset(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: no se pudo abrir el dataset: " << path << std::endl;
        return {};
    }

    std::vector<std::vector<double>> pts;
    std::string line;
    while (std::getline(file, line)) {
        for (char& c : line) {
            if (c == ',' || c == ';') c = ' ';
        }
        std::stringstream ss(line);
        double x, y;
        // Las lineas con un solo numero (como el conteo N en la primera linea) se omiten automaticamente
        if (ss >> x >> y) {
            pts.push_back({x, y});
        }
    }
    return pts;
}

void printHull(const std::vector<std::vector<double>>& hull) {
    std::cout << "[";
    for (size_t i = 0; i < hull.size(); i++) {
        if (i > 0) std::cout << ", ";
        std::cout << "[" << hull[i][0] << ", " << hull[i][1] << "]";
    }
    std::cout << "]" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <coordenadas_resultantes>.csv [dataset.txt]" << std::endl;
        return 1;
    }

    std::vector<std::vector<double>> points;
    if (argc >= 3) {
        points = loadDataset(argv[2]);
        if (points.empty()) {
            return 1;
        }
        std::cout << "Dataset cargado (" << points.size() << " puntos): " << argv[2] << std::endl;
    } else {
        points = {{-2, 1}, {4, 4}, {1, -2}, {1, 1}, {2, 1}, {-1, 3}, {3, -1}};
        std::cout << "Usando conjunto de puntos por defecto (" << points.size() << " puntos)." << std::endl;
    }

    std::ofstream coords(argv[1]);
    if (!coords.is_open()) {
        std::cerr << "Error: no se pudo crear el archivo CSV: " << argv[1] << std::endl;
        return 1;
    }
    coords << "step,type,x,y,changed\n";

    // 1. Calculo inicial del Convex Hull con Divide & Conquer O(n log n)
    std::vector<std::vector<double>> hull = dnc::findConvexHull(points);

    std::cout << "Convex Hull inicial (" << hull.size() << " vertices): ";
    printHull(hull);

    // Exportar paso 0 (estado inicial)
    for (const auto& p : points) {
        coords << "0,point," << p[0] << "," << p[1] << ",0\n";
    }
    for (const auto& p : hull) {
        coords << "0,hull," << p[0] << "," << p[1] << ",0\n";
    }

    // 2. Insercion incremental interactiva de nuevos puntos en O(h)
    std::cout << "\nInserta nuevos puntos en formato 'x y' (o escribe 'q' para terminar):" << std::endl;
    int step = 0;
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;

        // Recortar espacios iniciales
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) continue;
        if (line[first] == 'q' || line[first] == 'Q') break;

        for (char& c : line) {
            if (c == ',' || c == ';') c = ' ';
        }
        std::stringstream ss(line);
        double x, y;
        if (!(ss >> x >> y)) {
            std::cout << "Entrada invalida. Ingresa dos numeros 'x y' o 'q' para salir." << std::endl;
            continue;
        }

        step++;
        std::vector<std::vector<double>> oldHull = hull;
        bool changed = dnc::insertPoint(hull, {x, y});

        std::cout << "[Paso " << step << "] Punto (" << x << ", " << y << "): "
                  << (changed ? "CAMBIO el Convex Hull" : "NO cambio el Convex Hull (interior o en el borde)")
                  << " | Vertices: " << hull.size() << std::endl;

        int changedFlag = changed ? 1 : 0;
        for (const auto& p : oldHull) {
            coords << step << ",old_hull," << p[0] << "," << p[1] << "," << changedFlag << "\n";
        }
        coords << step << ",new_point," << x << "," << y << "," << changedFlag << "\n";
        for (const auto& p : hull) {
            coords << step << ",hull," << p[0] << "," << p[1] << "," << changedFlag << "\n";
        }
    }

    coords.close();
    std::cout << "\nResultados guardados en " << argv[1] << " (" << step << " inserciones registradas)." << std::endl;
    return 0;
}
