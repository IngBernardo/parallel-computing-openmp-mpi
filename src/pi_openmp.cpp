#include <iostream>
#include <chrono>
#include <omp.h>

int main() {

    long long n = 100000000;

    double step = 1.0 / n;

    double sum = 0.0;

    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for reduction(+:sum)

    for (long long i = 0; i < n; i++) {

        double x = (i + 0.5) * step;

        sum += 4.0 / (1.0 + x * x);
    }

    double pi = step * sum;

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    std::cout << "====================================\n";
    std::cout << "          PARALLEL PI - OPENMP\n";
    std::cout << "====================================\n";

    std::cout << "Intervalos: " << n << "\n";
    std::cout << "Hilos:      " << omp_get_max_threads() << "\n";
    std::cout << "PI:         " << pi << "\n";
    std::cout << "Tiempo:     " << elapsed.count()
              << " segundos\n";

    return 0;
}