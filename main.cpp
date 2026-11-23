#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "jarvis_march.cpp"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <coordenadas_resultantes>.csv" << std::endl;
        return 1;
    }

    std::ofstream coords;
    coords.open(argv[1]);
    coords << "type,x,y" << std::endl;

    std::vector<std::vector<double>> points = {{-2, 1}, {4, 4}, {1, -2}, {1, 1}, {2, 1}};

    std::vector<std::vector<double>> res = jarvis::findConvexHull(points);
    std::sort(res.begin(), res.end());

    std::cout << "[";
    for (int i = 0; i < res.size(); i++) {
        if (i > 0)
            std::cout << ", ";
        std::cout << "[" << res[i][0] << ", " << res[i][1] << "]";
    }
    std::cout << "]" << std::endl;

    for (const auto& point : points) {
        coords << "point," << point[0] << "," << point[1] << std::endl;
    }

    for (const auto& point : res) {
        coords << "hull," << point[0] << "," << point[1] << std::endl;
    }

    coords.close();

    return 0;
}