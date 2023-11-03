#include <bits/stdc++.h>

using namespace std;

#define MAX_DEPTH 2 // |threads| = 4^depth
#define MIN_SIZE 64 // 1=off 

int t;

struct Matrix
{
    int *M;
    int offset2d;
    int size;
    int bx, by;

    void partition(Matrix &A11, Matrix &A12, Matrix &A21, Matrix &A22)
    {
        A11 = {M, offset2d, size / 2, bx, by};
        A12 = {M, offset2d, size / 2, bx + size / 2, by};
        A21 = {M, offset2d, size / 2, bx, by + size / 2};
        A22 = {M, offset2d, size / 2, bx + size / 2, by + size / 2};
    }

    int &operator()(int x, int y)
    {
        return M[(y + by) * offset2d + (x + bx)];
    }
};

void strassen(Matrix &A, Matrix &B, Matrix &C, int depth = 0)
{
    int size = A.size;
    if (depth >= MAX_DEPTH || size <= MIN_SIZE)
    {
        for (int y = 0; y < size; ++y)
        {
            for (int x = 0; x < size; ++x)
            {
                int prod = 0;
                for (int k = 0; k < size; ++k)
                    prod += A(k, y) * B(x, k);
                C(x, y) = prod;
            }
        }
    }
    else
    {
        Matrix A11, A12, A21, A22;
        A.partition(A11, A12, A21, A22);
        Matrix B11, B12, B21, B22;
        B.partition(B11, B12, B21, B22);
        Matrix C11, C12, C21, C22;
        C.partition(C11, C12, C21, C22);

        Matrix T{new int[size * size], size, size, 0, 0};
        for (int y = 0; y < size; ++y)
            for (int x = 0; x < size; ++x)
                T(x, y) = 0;
        Matrix T11, T12, T21, T22;
        T.partition(T11, T12, T21, T22);

        thread threads[8] = {
            thread([&A11, &B11, &C11, depth]()
                   { strassen(A11, B11, C11, depth + 1); }),
            thread([&A11, &B12, &C12, depth]()
                   { strassen(A11, B12, C12, depth + 1); }),
            thread([&A21, &B11, &C21, depth]()
                   { strassen(A21, B11, C21, depth + 1); }),
            thread([&A21, &B12, &C22, depth]()
                   { strassen(A21, B12, C22, depth + 1); }),

            thread([&A12, &B21, &T11, depth]()
                   { strassen(A12, B21, T11, depth + 1); }),
            thread([&A12, &B22, &T12, depth]()
                   { strassen(A12, B22, T12, depth + 1); }),
            thread([&A22, &B21, &T21, depth]()
                   { strassen(A22, B21, T21, depth + 1); }),
            thread([&A22, &B22, &T22, depth]()
                   { strassen(A22, B22, T22, depth + 1); })};
        for (int i = 0; i < 8; ++i)
            threads[i].join();

        for (int y = 0; y < size; ++y)
            for (int x = 0; x < size; ++x)
                C(x, y) += T(x, y);

        delete[] T.M;
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

    Matrix A{new int[p2 * p2], p2, p2, 0, 0};
    Matrix B{new int[p2 * p2], p2, p2, 0, 0};
    Matrix C{new int[p2 * p2], p2, p2, 0, 0};
    Matrix T{new int[p2 * p2], p2, p2, 0, 0};

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
            C(x, y) = 0, T(x, y) = 0;
    }

    strassen(A, B, C);
    for (int y = 0; y < n; ++y)
    {
        for (int x = 0; x < n; ++x)
            cout << C(x, y) << ' ';
        cout << '\n';
    }
    delete[] A.M;
    delete[] B.M;
    delete[] C.M;
    delete[] T.M;
}