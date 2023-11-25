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
            res = float(f.readline().strip('\n'))
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


size = 4096
n_colors = ['blue', 'green', 'purple', 'brown',  'yellow']
s_colors = ['orange', 'red', 'pink', 'grey', 'violet']
a_colors = n_colors+s_colors

naive = ['omp_naive', 'omp_collapse',
         'omp_reversed', 'mpi_naive2d', 'mpi_naive1d']
strassen = ['mpi_strassen', 'omp_strassen1d', 'omp_strassen2d',
            'omp_strassen2dt', 'omp_strassen7']
all_algo = naive+strassen
plot(size, naive, n_colors)
plot(size, strassen, s_colors)
plot(size, all_algo, a_colors)


plt.show()
