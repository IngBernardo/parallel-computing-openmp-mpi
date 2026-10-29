#include <iostream>
#include <chrono>
#include <omp.h>

int main() {
    const long long n = 100000000;
    volatile double resultado = 0.0;
    double suma = 0.0;

    // Prueba secuencial
    auto inicio_seq = std::chrono::high_resolution_clock::now();

    for (long long i = 1; i <= n; i++) {
        suma += 1.0 / i;
    }

    auto fin_seq = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> tiempo_seq = fin_seq - inicio_seq;
    resultado = suma;

    // Prueba paralela con OpenMP
    suma = 0.0;

    auto inicio_omp = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for reduction(+:suma)
    for (long long i = 1; i <= n; i++) {
        suma += 1.0 / i;
    }

    auto fin_omp = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> tiempo_omp = fin_omp - inicio_omp;
    resultado = suma;

    std::cout << "===== COMPARACION OPENMP =====\n";
    std::cout << "Iteraciones: " << n << "\n";
    std::cout << "Hilos OpenMP: " << omp_get_max_threads() << "\n";
    std::cout << "Resultado: " << resultado << "\n";
    std::cout << "Tiempo secuencial: " << tiempo_seq.count() << " segundos\n";
    std::cout << "Tiempo OpenMP: " << tiempo_omp.count() << " segundos\n";

    if (tiempo_omp.count() > 0) {
        std::cout << "Speedup: "
                  << tiempo_seq.count() / tiempo_omp.count()
                  << "x\n";
    }

    return 0;
}
