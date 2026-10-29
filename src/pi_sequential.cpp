#include <iostream>
#include <chrono>

int main() {

    
    long long n = 100000000; //Cien Millones de intervalos y utilizamos long long para que no se desborde el valor de n

    
    double step = 1.0 / n;

    
    double sum = 0.0;

        auto start = std::chrono::high_resolution_clock::now();

    
    for (long long i = 0; i < n; i++) {

            double x = (i + 0.5) * step;

            sum += 4.0 / (1.0 + x * x);   //Esta es la funcion para calcular PI que es 4 entre (1 mas x al cuadrado)
    }

    
    double pi = step * sum;

        auto end = std::chrono::high_resolution_clock::now();

    
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "====================================\n";
    std::cout << "       PARALLEL PI - SEQUENTIAL\n";
    std::cout << "====================================\n";

    std::cout << "Intervalos: " << n << "\n";
    std::cout << "PI:         " << pi << "\n";
    std::cout << "Tiempo:     " << elapsed.count()
              << " segundos\n";

    return 0;
}