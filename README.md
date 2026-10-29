# Parallel Computing with OpenMP and MPI

Parallel programming project that explores different parallelization strategies for numerical computation. It compares sequential, OpenMP (shared memory), MPI (distributed memory), and hybrid (OpenMP + MPI) approaches using PI calculation via numerical integration and Mandelbrot set generation.

The entire environment runs inside Docker containers orchestrated with Docker Compose, simulating a multi-node cluster with one master and two worker nodes.

## Project Structure

```
├── Docker/
│   ├── dockerfile          # Ubuntu 24.04 image with OpenMPI and build tools
│   └── entrypoint.sh       # Container startup script (SSH + key setup)
├── src/
│   ├── pi_sequential.cpp   # PI calculation - sequential baseline
│   ├── pi_openmp.cpp       # PI calculation - OpenMP parallelization
│   ├── pi_mpi.cpp          # PI calculation - MPI distributed across nodes
│   ├── pi_hybrid.cpp       # PI calculation - MPI + OpenMP combined
│   ├── hello_openmp.cpp    # Basic OpenMP parallel for demo
│   ├── comparacion_openmp.cpp  # Sequential vs OpenMP speedup benchmark (harmonic sum)
│   ├── prueba_openmp.cpp       # OpenMP thread detection test
│   ├── mandelbrot_openmp.cpp   # Mandelbrot set with OpenMP parallel for + dynamic scheduling
│   └── mandelbrot_regiones.cpp # Mandelbrot + dithering with SPMD region-based parallelism
├── Results/                # Generated images (excluded from git)
├── docker-compose.yml      # 3-node cluster: master + 2 workers
└── README.md
```

## PI Calculation

All four PI programs approximate PI using the integral of `4 / (1 + x²)` over `[0, 1]` with 100 million intervals.

| Program | Parallelization | Key concept |
|---------|----------------|-------------|
| `pi_sequential` | None | Baseline timing |
| `pi_openmp` | `#pragma omp parallel for reduction(+:sum)` | Shared memory, automatic work distribution |
| `pi_mpi` | `MPI_Reduce` across processes | Distributed memory, each process handles `n/size` intervals |
| `pi_hybrid` | MPI between nodes + OpenMP within each node | Combines both paradigms |

## Mandelbrot Set

| Program | Approach | Details |
|---------|----------|---------|
| `mandelbrot_openmp` | `parallel for schedule(dynamic)` | 2000x2000 image, max 500 iterations. Dynamic scheduling balances load across threads. Outputs PPM image. |
| `mandelbrot_regiones` | SPMD with `#pragma omp parallel` region | Based on Example 2.10 from *Parallel Programming in OpenMP* (Chandra et al.). Each thread computes its own strip of rows (Mandelbrot + dithering). Compares sequential vs parallel timing. |

## Prerequisites

- Docker and Docker Compose
- For native compilation: `g++` with OpenMP support, OpenMPI

## Quick Start (Docker)

```bash
# Build and start the cluster
docker-compose up -d --build

# Enter the master node
docker exec -it mpi-master bash

# Switch to mpiuser
su - mpiuser
cd src/
```

### Compile and Run

**PI - Sequential:**
```bash
g++ -O2 pi_sequential.cpp -o pi_sequential
./pi_sequential
```

**PI - OpenMP:**
```bash
g++ -O2 -fopenmp pi_openmp.cpp -o pi_openmp
./pi_openmp
```

**PI - MPI (across 3 nodes):**
```bash
mpicxx -O2 pi_mpi.cpp -o pi_mpi
mpirun --host master,worker1,worker2 -np 3 ./pi_mpi
```

**PI - Hybrid (MPI + OpenMP):**
```bash
mpicxx -O2 -fopenmp pi_hybrid.cpp -o pi_hybrid
mpirun --host master,worker1,worker2 -np 3 ./pi_hybrid
```

**Mandelbrot - OpenMP:**
```bash
g++ -O2 -fopenmp mandelbrot_openmp.cpp -o mandelbrot_openmp
./mandelbrot_openmp
```

**Mandelbrot - Region-based (SPMD):**
```bash
g++ -O2 -fopenmp mandelbrot_regiones.cpp -o mandelbrot_regiones
./mandelbrot_regiones
```

### Control Thread Count

```bash
OMP_NUM_THREADS=4 ./pi_openmp
OMP_NUM_THREADS=2 ./mandelbrot_regiones
```

## Native Compilation (Ubuntu / WSL)

```bash
sudo apt update && sudo apt install -y build-essential openmpi-bin libopenmpi-dev

# Then use the same compile and run commands as above
```

## Expected Output

**PI calculation (OpenMP):**
```
====================================
          PARALLEL PI - OPENMP
====================================
Intervalos: 100000000
Hilos:      12
PI:         3.14159
Tiempo:     0.038 segundos
```

**Mandelbrot regions:**
```
===== EJEMPLO 2.10: MANDELBROT + DITHERING =====
Matriz: 2000x2000, maxiter = 200
Hilos usados   : 12
Tiempo serial  : 1580.42 ms
Tiempo paralelo: 310.67 ms
Speedup        : 5.08x
Resultados iguales: SI
```

## Technologies

- **C++** with `<chrono>` for timing
- **OpenMP** — shared memory parallelism (`parallel for`, `reduction`, `schedule(dynamic)`, SPMD regions)
- **MPI** (OpenMPI) — distributed memory parallelism (`MPI_Reduce`, `MPI_Wtime`)
- **Docker Compose** — multi-container cluster simulation
