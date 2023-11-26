#!/usr/bin/python3

from matplotlib import pyplot as plt
import numpy as np
import re
import os

benchmark = {}

for algo in os.listdir('time'):
    for test in os.listdir(f'time/{algo}'):
        path = f'time/{algo}/{test}'
        if algo not in benchmark:
            benchmark[algo] = {}
        size = int(re.match('(.+)_', test).group(1))
        if size not in benchmark[algo]:
            benchmark[algo][size] = []
        with open(path, 'r') as f:
            try:
                res = float(f.readline().strip('\n'))
            except:
                print(path)
                continue
            benchmark[algo][size] += [res]

for algo in benchmark.keys():
    for size, res in benchmark[algo].items():
        benchmark[algo][size] = np.mean(benchmark[algo][size])


def plot(size, algorithms, colors):
    results = [int(benchmark[algo][size]) for algo in algorithms]

    fig, ax = plt.subplots(layout='constrained')
    ax.set_yscale("log")
    ax.bar(algorithms, results, color=colors, label=algorithms,
           tick_label=[f'{r}ms' for r in results])
    ax.set_title(f"Benchmark for n={size}")
    ax.legend()


n_colors = ['blue', 'green', 'brown', 'purple',  'violet']
s_colors = ['pink', 'yellow', 'gray', 'red', 'orange']
f_colors = ['blue', 'red', 'orange']

naive = ['omp_naive', 'omp_collapsed',
         'omp_transposed', 'mpi_naive2d', 'mpi_naive1d']
strassen = ['mpi_strassen', 'omp_strassen1d', 'omp_strassen2d',
            'omp_strassen2dt', 'omp_strassen7']
final = ['omp_transposed', 'omp_strassen2dt', 'omp_strassen7']

plot(1024, naive, n_colors)
plot(4096, naive, n_colors)

plot(1024, strassen, s_colors)
plot(4096, strassen, s_colors)

plot(1024, final, f_colors)
plot(16384, final, f_colors)


plt.show()
