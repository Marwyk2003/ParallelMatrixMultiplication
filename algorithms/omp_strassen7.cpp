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

void freePart(int **A11, int **A12, int **A21, int **A22)
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

void Add(int **A, int **B, int **C, int size)
{
#pragma omp parallel for collapse(2)
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x)
            C[y][x] = A[y][x] + B[y][x];
}

void Sub(int **A, int **B, int **C, int size)
{
#pragma omp parallel for collapse(2)
    for (int y = 0; y < size; ++y)
        for (int x = 0; x < size; ++x)
            C[y][x] = A[y][x] - B[y][x];
}

void strassen(int **A, int **B, int **C, int size)
{
    if (size <= MIN_SIZE)
    {
        for (int y = 0; y < size; ++y)
        {
            for (int x = 0; x < size; ++x)
            {
                int prod = 0;
                for (int k = 0; k < size; ++k)
                    prod += A[y][k] * B[k][x];
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

        // Alloc all M matrix
        int *M1arr = new int[nsize * nsize]{0};
        int *M2arr = new int[nsize * nsize]{0};
        int *M3arr = new int[nsize * nsize]{0};
        int *M4arr = new int[nsize * nsize]{0};
        int *M5arr = new int[nsize * nsize]{0};
        int *M6arr = new int[nsize * nsize]{0};
        int *M7arr = new int[nsize * nsize]{0};

        int **M1 = alloc2d(M1arr, nsize);
        int **M2 = alloc2d(M2arr, nsize);
        int **M3 = alloc2d(M3arr, nsize);
        int **M4 = alloc2d(M4arr, nsize);
        int **M5 = alloc2d(M5arr, nsize);
        int **M6 = alloc2d(M6arr, nsize);
        int **M7 = alloc2d(M7arr, nsize);

#pragma omp task
        {
            int *Larr = new int[nsize * nsize]{0};
            int *Rarr = new int[nsize * nsize]{0};
            int **L = alloc2d(Larr, nsize);
            int **R = alloc2d(Rarr, nsize);

            Add(A11, A22, L, nsize);
            Add(B11, B22, R, nsize);
            strassen(L, R, M1, nsize);

            delete[] L;
            delete[] R;
            delete[] Larr;
            delete[] Rarr;
        }
#pragma omp task
        {
            int *Larr = new int[nsize * nsize]{0};
            int **L = alloc2d(Larr, nsize);

            Add(A21, A22, L, nsize);
            strassen(L, B11, M2, nsize);

            delete[] L;
            delete[] Larr;
        }
#pragma omp task
        {
            int *Rarr = new int[nsize * nsize]{0};
            int **R = alloc2d(Rarr, nsize);

            Sub(B12, B22, R, nsize);
            strassen(A11, R, M3, nsize);

            delete[] R;
            delete[] Rarr;
        }
#pragma omp task
        {
            int *Rarr = new int[nsize * nsize]{0};
            int **R = alloc2d(Rarr, nsize);

            Sub(B21, B11, R, nsize);
            strassen(A22, R, M4, nsize);

            delete[] R;
            delete[] Rarr;
        }
#pragma omp task
        {
            int *Larr = new int[nsize * nsize]{0};
            int **L = alloc2d(Larr, nsize);

            Add(A11, A12, L, nsize);
            strassen(L, B22, M5, nsize);

            delete[] L;
            delete[] Larr;
        }
#pragma omp task
        {
            int *Larr = new int[nsize * nsize]{0};
            int *Rarr = new int[nsize * nsize]{0};
            int **L = alloc2d(Larr, nsize);
            int **R = alloc2d(Rarr, nsize);

            Sub(A21, A11, L, nsize);
            Add(B11, B12, R, nsize);
            strassen(L, R, M6, nsize);

            delete[] L;
            delete[] R;
            delete[] Larr;
            delete[] Rarr;
        }
#pragma omp task
        {
            int *Larr = new int[nsize * nsize]{0};
            int *Rarr = new int[nsize * nsize]{0};
            int **L = alloc2d(Larr, nsize);
            int **R = alloc2d(Rarr, nsize);

            Sub(A12, A22, L, nsize);
            Add(B21, B22, R, nsize);
            strassen(L, R, M7, nsize);

            delete[] L;
            delete[] R;
            delete[] Larr;
            delete[] Rarr;
        }
#pragma omp taskwait

// combine results
#pragma omp parallel for collapse(2)
        for (int y = 0; y < nsize; ++y)
            for (int x = 0; x < nsize; ++x)
                C11[y][x] = M1[y][x] + M4[y][x] - M5[y][x] + M7[y][x];

#pragma omp parallel for collapse(2)
        for (int y = 0; y < nsize; ++y)
            for (int x = 0; x < nsize; ++x)
                C12[y][x] = M3[y][x] + M5[y][x];

#pragma omp parallel for collapse(2)
        for (int y = 0; y < nsize; ++y)
            for (int x = 0; x < nsize; ++x)
                C21[y][x] = M2[y][x] + M4[y][x];

#pragma omp parallel for collapse(2)
        for (int y = 0; y < nsize; ++y)
            for (int x = 0; x < nsize; ++x)
                C22[y][x] = M1[y][x] - M2[y][x] + M3[y][x] + M6[y][x];

        freePart(A11, A12, A21, A22);
        freePart(B11, B12, B21, B22);
        freePart(C11, C12, C21, C22);

        // free all M matrix
        delete[] M1;
        delete[] M2;
        delete[] M3;
        delete[] M4;
        delete[] M5;
        delete[] M6;
        delete[] M7;

        delete[] M1arr;
        delete[] M2arr;
        delete[] M3arr;
        delete[] M4arr;
        delete[] M5arr;
        delete[] M6arr;
        delete[] M7arr;
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