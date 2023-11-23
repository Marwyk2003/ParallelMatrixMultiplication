// inspired by: https://github.com/spectre900/Parallel-Strassen-Algorithm/blob/master/omp_strassen.cpp

#include <bits/stdc++.h>
#include <omp.h>
#include "env.h"

using namespace std;
using namespace chrono;

int **alloc2d(int *M1, int size)
{
    int **M = new int *[size];
    for (int i = 0; i < size; ++i)
        M[i] = M1 + i * size;
    return M;
}

int **offset(int **M1, int ox, int oy, int size)
{
    int **M = new int *[size];
    for (int i = 0; i < size; ++i)
        M[i] = M1[i + oy] + ox;
    return M;
}

void freePart(int** A11, int** A12, int** A21, int** A22)
{
    delete[] A11;
    delete[] A12;
    delete[] A21;
    delete[] A22;
}

tuple<int **, int **, int **, int **> partition(int **M, int s)
{
    return {offset(M, 0, 0, s), offset(M, s, 0, s), offset(M, 0, s, s), offset(M, s, s, s)};
}

void strassen(int **A, int **B, int **C, int size)
{
    if (size <= MIN_SIZE)
    {
#pragma omp parallel for collapse(2)
        for (int y = 0; y < size; ++y)
        {
            for (int x = 0; x < size; ++x)
            {
                int prod = 0;
                for (int k = 0; k < size; ++k)
                    prod += A[y][k] * B[x][k];
                C[y][x] += prod;
            }
        }
    }
    else
    {
        int nsize = size / 2;
        const auto &[A11, A12, A21, A22] = partition(A, nsize);
        const auto &[B11, B12, B21, B22] = partition(B, nsize);
        const auto &[C11, C12, C21, C22] = partition(C, nsize);

#pragma omp task
        {
            strassen(A11, B11, C11, nsize);
            strassen(A12, B21, C11, nsize);
        }
#pragma omp task
        {
            strassen(A11, B12, C12, nsize);
            strassen(A12, B22, C12, nsize);
        }
#pragma omp task
        {
            strassen(A21, B11, C21, nsize);
            strassen(A22, B21, C21, nsize);
        }
#pragma omp task
        {
            strassen(A21, B12, C22, nsize);
            strassen(A22, B22, C22, nsize);
        }
#pragma omp taskwait

        freePart(A11, A12, A21, A22);
        freePart(B11, B12, B21, B22);
        freePart(C11, C12, C21, C22);
    }
}

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    int n;
    cin >> n;
    int p2 = 1;
    while (p2 < n)
        p2 *= 2;

    int *Aarr = new int[p2 * p2];
    int *Barr = new int[p2 * p2];
    int *Carr = new int[p2 * p2];

    int **A = alloc2d(Aarr, p2);
    int **B = alloc2d(Barr, p2);
    int **C = alloc2d(Carr, p2);

    for (int y = 0; y < n; ++y)
    {
        for (int x = 0; x < n; ++x)
            cin >> A[y][x];
        for (int x = p2; x < p2; ++x)
            A[y][x] = 0;
    }
    for (int y = 0; y < n; ++y)
    {
        for (int x = 0; x < n; ++x)
            cin >> B[y][x];
        for (int x = p2; x < p2; ++x)
            B[y][x] = 0;
    }
    for (int y = n; y < p2; ++y)
        for (int x = 0; x < p2; ++x)
            A[y][x] = 0, B[y][x] = 0;
    for (int y = 0; y < p2; ++y)
        for (int x = 0; x < p2; ++x)
            C[y][x] = 0;

    auto t1 = high_resolution_clock::now();
    omp_set_num_threads(THREAD_NUM);

#pragma omp parallel
    {
#pragma omp single
        {
            strassen(A, B, C, p2);
        }
    }

    auto t2 = high_resolution_clock::now();
    duration<double, milli> time = t2 - t1;
    cerr << time.count() << '\n';

    for (int y = 0; y < n; ++y)
    {
        for (int x = 0; x < n; ++x)
            cout << C[y][x] << ' ';
        cout << '\n';
    }

    delete[] A;
    delete[] B;
    delete[] C;
    delete[] Aarr;
    delete[] Barr;
    delete[] Carr;
}