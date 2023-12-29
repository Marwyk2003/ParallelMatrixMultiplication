# Parallel Matrix Multiplication
Benchmarking various algorithms for matrix multiplication
## Usage
### Generate input files:
```console
./gen 1024 4096
```
### Benchmark algorithms
```console
./benchmark mpi_naive1d.cpp omp_transposed.cpp omp_strassen7.cpp
```
Or benchmark every .cpp file in `algoritms/`
```console
./benchmark
```
To edit hyper parameters e.g. `NUM_THREADS` see `algorithms/env.h`
### Clear
To clear tests, bin and time use
```console
./clear
```
### Plot
You can plot benchmarked time using
```console
./plot.py
```