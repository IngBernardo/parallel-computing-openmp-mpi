#include <iostream>
#include <vector>
#include <omp.h>

// Equivalente a mandel_val del libro: cuantas iteraciones aguanta el punto (x, y)
int mandel_val(double x, double y, int maxiter) {
    double zr = x, zi = y;
    int k;
    for (k = 0; k < maxiter && zr * zr + zi * zi < 4.0; k++) {
        double tmp = zr * zr - zi * zi + x;
        zi = 2.0 * zr * zi + y;
        zr = tmp;
    }
    return k;
}

int main() {
    const int m = 2000;      // filas (direccion i)
    const int n = 2000;      // columnas (direccion j)
    const int maxiter = 200;

    std::vector<int>    depth(m * n), depth_seq(m * n);
    std::vector<double> dith_depth(m * n, 0.0), dith_seq(m * n, 0.0);

    // ===== Version secuencial (Ejemplo 2.9) =====
    double t0 = omp_get_wtime();
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++) {
            double x = (double)i / m * 4.0 - 2.0;
            double y = (double)j / n * 4.0 - 2.0;
            depth_seq[i * n + j] = mandel_val(x, y, maxiter);
        }
    for (int i = 0; i < m; i++)
        for (int j = 1; j < n - 1; j++)
            dith_seq[i * n + j] = 0.5 * depth_seq[i * n + j] +
                0.25 * (depth_seq[i * n + j - 1] + depth_seq[i * n + j + 1]);
    double t_seq = omp_get_wtime() - t0;

    // ===== Version paralela con region paralela (Ejemplo 2.10) =====
    int hilos = 0;
    t0 = omp_get_wtime();

    #pragma omp parallel
    {
        // Declaradas dentro de la region: son privadas de cada hilo
        int my_thread = omp_get_thread_num();
        int nthreads  = omp_get_num_threads();
        int my_width  = m / nthreads;
        int i_start   = my_thread * my_width;
        // El ultimo hilo se queda con las filas sobrantes si m no es divisible
        int i_end     = (my_thread == nthreads - 1) ? m : i_start + my_width;

        if (my_thread == 0) hilos = nthreads;

        // Fase 1: Mandelbrot de mi franja de filas
        for (int i = i_start; i < i_end; i++)
            for (int j = 0; j < n; j++) {
                double x = (double)i / m * 4.0 - 2.0;
                double y = (double)j / n * 4.0 - 2.0;
                depth[i * n + j] = mandel_val(x, y, maxiter);
            }

        // Fase 2: dithering de mi misma franja, sin esperar a los demas hilos
        // (solo uso vecinos en j, que estan en mis propias filas)
        for (int i = i_start; i < i_end; i++)
            for (int j = 1; j < n - 1; j++)
                dith_depth[i * n + j] = 0.5 * depth[i * n + j] +
                    0.25 * (depth[i * n + j - 1] + depth[i * n + j + 1]);
    }

    double t_par = omp_get_wtime() - t0;

    // Verificar que ambas versiones dan lo mismo
    bool iguales = (dith_depth == dith_seq);

    std::cout << "===== EJEMPLO 2.10: MANDELBROT + DITHERING =====\n";
    std::cout << "Matriz: " << m << "x" << n << ", maxiter = " << maxiter << "\n";
    std::cout << "Hilos usados   : " << hilos << "\n";
    std::cout << "Tiempo serial  : " << t_seq * 1000 << " ms\n";
    std::cout << "Tiempo paralelo: " << t_par * 1000 << " ms\n";
    std::cout << "Speedup        : " << t_seq / t_par << "x\n";
    std::cout << "Resultados iguales: " << (iguales ? "SI" : "NO") << "\n";

    return 0;
}
