#include <bits/stdc++.h>
#include <omp.h>
#include "env.h"

using namespace std;
using namespace chrono;

struct Matrix
{
    int *M;
    int offset2d;
    int bx, by;

    void partition(Matrix &A11, Matrix &A12, Matrix &A21, Matrix &A22, int nsize)
    {
        A11 = {M, offset2d, bx, by};
        A12 = {M, offset2d, bx + nsize, by};
        A21 = {M, offset2d, bx, by + nsize};
        A22 = {M, offset2d, bx + nsize, by + nsize};
    }

    int &operator()(int x, int y)
    {
        return M[(y + by) * offset2d + (x + bx)];
    }
};

void strassen(Matrix &A, Matrix &B, Matrix &C, int size)
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
                    prod += A(k, y) * B(x, k);
                C(x, y) += prod;
            }
        }
    }
    else
    {
        int nsize = size / 2;
        Matrix A11, A12, A21, A22;
        A.partition(A11, A12, A21, A22, nsize);
        Matrix B11, B12, B21, B22;
        B.partition(B11, B12, B21, B22, nsize);
        Matrix C11, C12, C21, C22;
        C.partition(C11, C12, C21, C22, nsize);

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

    Matrix A{new int[p2 * p2], p2, 0, 0};
    Matrix B{new int[p2 * p2], p2, 0, 0};
    Matrix C{new int[p2 * p2], p2, 0, 0};

    for (int y = 0; y < n; ++y)
    {
        for (int x = 0; x < n; ++x)
            cin >> A(x, y);
        for (int x = p2; x < p2; ++x)
            A(x, y) = 0;
    }
    for (int y = 0; y < n; ++y)
    {
        for (int x = 0; x < n; ++x)
            cin >> B(x, y);
        for (int x = p2; x < p2; ++x)
            B(x, y) = 0;
    }
    for (int y = n; y < p2; ++y)
    {
        for (int x = 0; x < p2; ++x)
            A(x, y) = 0, B(x, y) = 0;
    }
    for (int y = 0; y < p2; ++y)
    {
        for (int x = 0; x < p2; ++x)
            C(x, y) = 0;
    }

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
    duration<double, std::milli> time = t2 - t1;
    cerr << time.count() << '\n';

    for (int y = 0; y < n; ++y)
    {
        for (int x = 0; x < n; ++x)
            cout << C(x, y) << ' ';
        cout << '\n';
    }

    delete[] A.M;
    delete[] B.M;
    delete[] C.M;
}