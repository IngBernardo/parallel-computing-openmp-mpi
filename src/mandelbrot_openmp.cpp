#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <omp.h>

// Numero complejo simple (reemplaza el "complex.h" del articulo)
struct Complejo {
    double r, i;
};

Complejo cuadrado(Complejo c) {
    return { c.r * c.r - c.i * c.i, 2.0 * c.r * c.i };
}

Complejo sumar(Complejo a, Complejo b) {
    return { a.r + b.r, a.i + b.i };
}

double magnitud2(Complejo c) {
    return c.r * c.r + c.i * c.i;
}

// Calcula el valor (0..1) de un pixel del conjunto de Mandelbrot
float calcularPixel(int x, int y, int w, int h, int max) {
    double fx = (double)x / w * 4.0 - 2.0;
    double fy = (double)y / h * 4.0 - 2.0;

    Complejo c = { fx, fy };
    Complejo c0 = c;

    int i;
    for (i = 0; i < max && magnitud2(c) < 4.0; i++) {
        c = cuadrado(c);
        c = sumar(c, c0);
    }

    return (float)i / (float)max;
}

// Guarda la imagen en formato PPM (reemplaza el "ppm.h" del articulo)
void guardarPPM(const char* nombre, const std::vector<float>& img, int w, int h) {
    std::ofstream archivo(nombre, std::ios::binary);
    archivo << "P6\n" << w << " " << h << "\n255\n";
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            unsigned char v = (unsigned char)(img[y * w + x] * 255);
            archivo << v << v << v;
        }
    }
}

int main() {
    const int w = 2000;
    const int h = 2000;
    const int max = 500;

    std::vector<float> img(w * h);

    // Version con OpenMP
    auto inicio_omp = std::chrono::high_resolution_clock::now();

    #pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            img[y * w + x] = calcularPixel(x, y, w, h, max);
        }
    }

    auto fin_omp = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> tiempo_omp = fin_omp - inicio_omp;

    // Version secuencial
    auto inicio_seq = std::chrono::high_resolution_clock::now();

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            img[y * w + x] = calcularPixel(x, y, w, h, max);
        }
    }

    auto fin_seq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> tiempo_seq = fin_seq - inicio_seq;

    std::cout << "===== MANDELBROT OPENMP =====\n";
    std::cout << "Imagen: " << w << "x" << h << " pixeles\n";
    std::cout << "Hilos OpenMP: " << omp_get_max_threads() << "\n";
    std::cout << "Con OMP    : " << tiempo_omp.count() << " ms\n";
    std::cout << "Sin OMP    : " << tiempo_seq.count() << " ms\n";
    std::cout << "Diferencia : " << tiempo_seq.count() - tiempo_omp.count() << " ms\n";
    std::cout << "Speedup    : " << tiempo_seq.count() / tiempo_omp.count() << "x\n";

    guardarPPM("../results/mandelbrot.ppm", img, w, h);
    std::cout << "Imagen guardada en results/mandelbrot.ppm\n";

    return 0;
}
