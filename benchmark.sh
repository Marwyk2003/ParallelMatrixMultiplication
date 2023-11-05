#/usr/bin/sh

cat algorithms/benchmark.txt | while read file; do
    bin=$(basename ${file%.*})
    echo "$bin:"
    g++ algorithms/$file -o bin/$bin -fopenmp -pthread
    for package in tests/*; do
        size=$(basename ${package%.*})
        echo -n "\t$size:\t"
        for test in tests/$size/*; do
            ti=$(basename ${test%.*})
            echo -n "$ti "
            mkdir -p out/$bin/ time/$bin/
            ./bin/$bin < $test > "out/$bin/${size}_$ti.out" 2> "time/$bin/${size}_$ti.txt"
        done
        echo "\tdone"
    done
done
