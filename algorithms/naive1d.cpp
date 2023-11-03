#include <bits/stdc++.h>
using namespace std;

#define THREAD_NUM 16

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    int n;
    cin >> n;
    vector<int> A(n * n), B(n * n);
    vector<int> C(n * n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            cin >> A[i * n + j];
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            cin >> B[j * n + i];

    thread threads[THREAD_NUM];
    for (int t = 0; t < THREAD_NUM; ++t)
    {
        threads[t] = thread([&A, &B, &C, n, t]()
                            {
        for (int i = t; i < n; i += THREAD_NUM)
            for (int j = 0; j < n; ++j)
            {
                int prod=0;
                for (int k = 0; k < n; ++k)
                    prod += A[i * n + k] * B[j * n + k]; 
                C[i*n+j]=prod;
            } });
    }
    for (int t = 0; t < THREAD_NUM; ++t)
        threads[t].join();

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
            cout << C[i * n + j] << ' ';
        cout << '\n';
    }
}