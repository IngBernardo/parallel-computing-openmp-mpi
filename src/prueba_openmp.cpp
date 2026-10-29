#include <omp.h>
#include <iostream>
int main() {
    std::cout << "Hilos disponibles: "
              << omp_get_max_threads() << "\n";
    return 0;
}
