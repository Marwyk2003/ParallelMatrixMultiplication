#!/usr/bin/bash

shopt -s nullglob

if [ "$#" -eq 0 ]; then
    files=(algorithms/*.cpp)
else
    files=("$@")
    for ((i = 0; i < ${#files[@]}; i++)); do
        files[$i]="algorithms/${files[$i]}"
    done
fi

for file in ${files[@]}; do
    echo $file
    bin=$(basename ${file%.*})
    echo "$bin:"
    g++ $file -o bin/$bin -O3 -fopenmp -pthread
    for package in tests/*; do
        size=$(basename ${package%.*})
        echo -ne "\t$size:\t"
        for test in tests/$size/*; do
            ti=$(basename ${test%.*})
            echo -n "$ti "
            mkdir -p out/$bin/ time/$bin/
            ./bin/$bin <$test >"out/$bin/${size}_$ti.out" 2>"time/$bin/${size}_$ti.txt"
        done
        echo -e "\tdone"
    done
done
