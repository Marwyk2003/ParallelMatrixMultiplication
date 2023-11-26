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

void mult(int **A, int **B, int **C, int size)
{
#pragma omp parallel for
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

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    int n;
    cin >> n;
    int *Aarr = new int[n * n]{0};
    int *Barr = new int[n * n]{0};
    int *Carr = new int[n * n]{0};

    int **A = alloc2d(Aarr, n);
    int **B = alloc2d(Barr, n);
    int **C = alloc2d(Carr, n);

    for (int y = 0; y < n; ++y)
        for (int x = 0; x < n; ++x)
            cin >> A[y][x];
    for (int y = 0; y < n; ++y)
        for (int x = 0; x < n; ++x)
            cin >> B[x][y];

    auto t1 = high_resolution_clock::now();
    omp_set_num_threads(THREAD_NUM);

    mult(A, B, C, n);

    auto t2 = high_resolution_clock::now();
    duration<double, milli> time = t2 - t1;
    cerr << time.count() << '\n';

    if (PRINT_OUT)
    {
        for (int y = 0; y < n; ++y)
        {
            for (int x = 0; x < n; ++x)
                cout << C[y][x] << ' ';
            cout << '\n';
        }
    }

    delete[] A;
    delete[] B;
    delete[] C;
    delete[] Aarr;
    delete[] Barr;
    delete[] Carr;
}