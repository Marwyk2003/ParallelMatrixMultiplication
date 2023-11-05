// inspired by: https://github.com/spectre900/Parallel-Strassen-Algorithm/blob/master/omp_strassen.cpp
// struct 1d strassen

#include <bits/stdc++.h>
#include <omp.h>

using namespace std;
using namespace chrono;

#define THREAD_NUM 16
#define MIN_SIZE 32

struct Matrix
{
    int **M;
    int size;

    Matrix() {}

    Matrix(int **M1, int bx, int by, int s)
    {
        size = s;
        M = new int *[size];
        for (int i = 0; i < size; ++i)
            M[i] = M1[i + by] + bx;
    }

    Matrix(int *M1, int s)
    {
        size = s;
        M = new int *[size];
        for (int i = 0; i < size; ++i)
            M[i] = M1 + i * size;
    }

    void partition(Matrix &A11, Matrix &A12, Matrix &A21, Matrix &A22)
    {
        int s = size / 2;
        A11 = Matrix(M, 0, 0, s);
        A12 = Matrix(M, s, 0, s);
        A21 = Matrix(M, 0, s, s);
        A22 = Matrix(M, s, s, s);
    }

    int &operator()(int x, int y)
    {
        return M[y][x];
    }

};

void freePart(Matrix &A11, Matrix &A12, Matrix &A21, Matrix &A22)
{
    delete[] A11.M;
    delete[] A12.M;
    delete[] A21.M;
    delete[] A22.M;
}

void strassen(Matrix &A, Matrix &B, Matrix &C, int size)
{
    if (size <= MIN_SIZE)
    {
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
        A.partition(A11, A12, A21, A22);
        Matrix B11, B12, B21, B22;
        B.partition(B11, B12, B21, B22);
        Matrix C11, C12, C21, C22;
        C.partition(C11, C12, C21, C22);

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

    Matrix A(Aarr, p2);
    Matrix B(Barr, p2);
    Matrix C(Carr, p2);

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
    duration<double, milli> time = t2 - t1;
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
    delete[] Aarr;
    delete[] Barr;
    delete[] Carr;
}