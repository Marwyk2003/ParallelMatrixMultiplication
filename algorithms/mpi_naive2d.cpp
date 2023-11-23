#include <bits/stdc++.h>
#include "env.h"

using namespace std;
using namespace chrono;

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    int n;
    cin >> n;
    vector<vector<int>> A(n, vector<int>(n)), B(n, vector<int>(n));
    vector<vector<int>> C(n, vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            cin >> A[i][j];
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            cin >> B[j][i];

    auto t1 = high_resolution_clock::now();

    thread threads[THREAD_NUM];
    for (int t = 0; t < THREAD_NUM; ++t)
    {
        threads[t] = thread([&A, &B, &C, n, t]()
                            {
        for (int i = t; i < n; i+=THREAD_NUM)
            for (int j = 0; j < n; ++j)
            {
                int prod=0;
                for (int k = 0; k < n; ++k)
                    prod += A[i][k] * B[j][k]; 
                C[i][j]=prod;
            } });
    }
    for (int t = 0; t < THREAD_NUM; ++t)
        threads[t].join();

    auto t2 = high_resolution_clock::now();
    duration<double, milli> time = t2 - t1;
    cerr << time.count() << '\n';

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
            cout << C[i][j] << ' ';
        cout << '\n';
    }
}