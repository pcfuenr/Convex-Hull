/** uhr: generic time performance tester
 * Author: LELE
 *
 * Things to set up:
 * 0. Includes: include all files to be tested,
 * 1. Time unit: in elapsed_time,
 * 2. What to write on time_data,
 * 3. The experiments: run over all points of the chosen dataset. */

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Include to be tested files here
#include "jarvis_march.cpp"
#include "d&c.cpp"
#include "graham.cpp"

inline void validate_input(int argc, char *argv[], std::int64_t& runs,
    std::string& algo, std::string& dataset_name,
    std::vector<std::vector<double>>& points)
{
    if (argc != 5) {
        std::cerr << "Usage: <filename> <RUNS> <ALGO> <DATASET_NAME>" << std::endl;
        std::cerr << "<filename> is the name of the file where performance data will be written." << std::endl;
        std::cerr << "It is recommended for <filename> to have .csv extension and it should not previously exist." << std::endl;
        std::cerr << "<RUNS>: numbers of runs per test case: should be >= 32." << std::endl;
        std::cerr << "<ALGO>: jarvis | dnc | graham" << std::endl;
        std::cerr << "<DATASET_NAME>: file name inside datasets/, for example: circulo_100.txt" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Read command line arguments
    try {
        runs = std::stoll(argv[2]);
        algo = argv[3];
        dataset_name = argv[4];
    } catch (std::invalid_argument const& ex) {
        std::cerr << "std::invalid_argument::what(): " << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    } catch (std::out_of_range const& ex) {
        std::cerr << "std::out_of_range::what(): " << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Validate arguments
    if (runs < 4) {
        std::cerr << "<RUNS> must be at least 4." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    if (algo != "jarvis" and algo != "dnc" and algo != "graham") {
        std::cerr << "<ALGO> must be: jarvis, dnc or graham." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::vector<std::string> files;
    if (DIR* dir = opendir("datasets")) {
        while (dirent* ent = readdir(dir)) {
            std::string name = ent->d_name;
            if (name != "." and name != "..")
                files.push_back("datasets/" + name);
        }
        closedir(dir);
    }
    std::sort(files.begin(), files.end());

    std::string selected_file;
    for (const std::string& file : files) {
        const std::string file_name = file.substr(file.find_last_of("/\\") + 1);
        if (file_name == dataset_name)
            selected_file = file;
    }

    if (selected_file.empty()) {
        std::cerr << "Unknown dataset name: " << dataset_name << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::ifstream data_file(selected_file);
    std::string line;
    while (std::getline(data_file, line)) {
        for (char& c : line)
            if (c == ',' or c == ';') c = ' ';
        std::stringstream ss(line);
        double x, y;
        if (ss >> x >> y)
            points.push_back({x, y});
    }

    if (points.empty()) {
        std::cerr << "Selected dataset file is empty or unreadable: " << selected_file << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

inline void display_progress(std::int64_t u, std::int64_t v)
{
    const double progress = u / double(v);
    const std::int64_t width = 70;
    const std::int64_t p = width * progress;
    std::int64_t i;

    std::cout << "\033[1m[";
    for (i = 0; i < width; i++) {
        if (i < p)
            std::cout << "=";
        else if (i == p)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] " << std::int64_t(progress * 100.0) << "%\r\033[0m";
    std::cout.flush();
}

inline void quartiles(std::vector<double>& data, std::vector<double>& q)
{
    q.resize(5);
    std::size_t n = data.size();
    std::size_t p;

    std::sort(data.begin(), data.end());

    if (n < 4) {
        std::cerr << "quartiles needs at least 4 data points." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Get min and max
    q[0] = data.front();
    q[4] = data.back();

    // Find median
    if (n % 2 == 1) {
        q[2] = data[n / 2];
    } else {
        p = n / 2;
        q[2] = (data[p - 1] + data[p]) / 2.0;
    }

    // Find lower and upper quartiles
    if (n % 4 >= 2) {
        q[1] = data[n / 4];
        q[3] = data[(3 * n) / 4];
    } else {
        p = n / 4;
        q[1] = 0.25 * data[p - 1] + 0.75 * data[p];
        p = (3 * n) / 4;
        q[3] = 0.75 * data[p - 1] + 0.25 * data[p];
    }
}

int main(int argc, char *argv[])
{
    // Validate and sanitize input
    std::int64_t runs;
    std::string algo;
    std::string dataset_name;
    std::vector<std::vector<double>> points;
    validate_input(argc, argv, runs, algo, dataset_name, points);

    // Set up clock variables
    std::int64_t n = points.size(), i;
    std::vector<double> times(runs);
    std::vector<double> q;
    double mean_time = 0, time_stdev = 0, dev;
    auto begin_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> elapsed_time = end_time - begin_time;

    // File to write time data
    std::ofstream time_data;
    time_data.open(argv[1]);
    time_data << "dataset,n,t_mean,t_stdev,t_Q0,t_Q1,t_Q2,t_Q3,t_Q4" << std::endl;

    std::string dataset_label = dataset_name;
    const std::size_t extension = dataset_label.find_last_of('.');
    if (extension != std::string::npos)
        dataset_label.erase(extension);

    // Begin testing
    std::cout << "\033[0;36mRunning tests...\033[0m" << std::endl << std::endl;

    // Run to compute elapsed time
    for (i = 0; i < runs; i++) {
        display_progress(i + 1, runs);

        begin_time = std::chrono::high_resolution_clock::now();
        // Function to test goes here
        if (algo == "jarvis")
            jarvis::findConvexHull(points);
        else if (algo == "dnc")
            dnc::findConvexHull(points);
        else if (algo == "graham")
            graham::findConvexHull(points);
        end_time = std::chrono::high_resolution_clock::now();

        elapsed_time = end_time - begin_time;
        times[i] = elapsed_time.count();

        mean_time += times[i];
    }

    // Compute statistics
    mean_time /= runs;

    for (i = 0; i < runs; i++) {
        dev = times[i] - mean_time;
        time_stdev += dev * dev;
    }

    time_stdev /= runs - 1; // Subtract 1 to get unbiased estimator
    time_stdev = std::sqrt(time_stdev);

    quartiles(times, q);

    time_data << dataset_label << "," << n << "," << mean_time << "," << time_stdev << ",";
    time_data << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << "," << q[4] << std::endl;

    // This is to keep loading bar after testing
    std::cout << std::endl << std::endl;
    std::cout << "\033[1;32mDone!\033[0m" << std::endl;

    time_data.close();

    return 0;
}
