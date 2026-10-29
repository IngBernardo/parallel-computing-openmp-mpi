#include <iostream>
#include <chrono>
#include <mpi.h>

int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);

    int rank;

    int size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long n = 100000000;

    double step = 1.0 / n;

    
    long long local_n = n / size;

    double local_sum = 0.0;

    double start = MPI_Wtime();

    for (long long i = 0; i < local_n; i++) {

        long long index = rank * local_n + i;

        double x = (index + 0.5) * step;

        local_sum += 4.0 / (1.0 + x * x);
    }

    double global_sum = 0.0;

    MPI_Reduce(
        &local_sum,
        &global_sum,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    double end = MPI_Wtime();

    if (rank == 0) {

        double pi = step * global_sum;

        std::cout << "====================================\n";
        std::cout << "           PARALLEL PI - MPI\n";
        std::cout << "====================================\n";

        std::cout << "Intervalos: " << n << "\n";
        std::cout << "Procesos:   " << size << "\n";
        std::cout << "PI:         " << pi << "\n";
        std::cout << "Tiempo:     " << end - start
                  << " segundos\n";
    }

    MPI_Finalize();

    return 0;
}