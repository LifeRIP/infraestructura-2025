#include <cmath>
#include <iostream>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;
using namespace tbb;
using namespace std::chrono;

void fillVector(vector<long> &v)
{
    for (int i = 0; i < v.size(); i++)
    {
        v[i] = i;
    }
}

void fillVectorTBB(vector<long> &v)
{
    parallel_for(
        blocked_range<int>(0, v.size()),
        [&](blocked_range<int> r)
        {
            for (int i = r.begin(); i != r.end(); i++)
            {
                v[i] = i;
            }
        });
}

double calcNormP(vector<long> v, long long p)
{
    double suma = 0;
    for (int i = 0; i < v.size(); i++)
    {
        suma += pow(abs(v[i]), p);
    }
    return sqrt(suma);
}

double calcNormPTBB(vector<long> v, long long p)
{
    double suma = parallel_reduce(
        blocked_range<int>(0, v.size()),
        0.0,
        [&](blocked_range<int> r, double local_suma) -> double
        {
            double sum = local_suma;
            for (int i = r.begin(); i != r.end(); i++)
            {
                sum += pow(abs(v[i]), p);
            }
            return sum;
        },
        std::plus<double>());
    return sqrt(suma);
}

int main()
{
    // Llenar vector secuencial
    auto start = chrono::system_clock::now();
    vector<long> v(1000000);
    fillVector(v);
    auto end = chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    printf("Tiempo llenado de ejecución secuencial: %ldms\n", duration.count());

    // Llenar vector con TBB
    start = chrono::system_clock::now();
    vector<long> vTBB(1000000);
    fillVectorTBB(vTBB);
    end = chrono::system_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    printf("Tiempo llenado de ejecución con TBB: %ldms\n", duration.count());

    // Calcular norma p secuencial
    start = chrono::system_clock::now();
    double norma = calcNormP(v, 2);
    end = chrono::system_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    printf("Tiempo cálculo norma p secuencial: %ldms\n", duration.count());
    printf("Norma p secuencial: %f\n", norma);

    // Calcular norma p con TBB
    start = chrono::system_clock::now();
    double normaTBB = calcNormPTBB(vTBB, 2);
    end = chrono::system_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    printf("Tiempo cálculo norma p con TBB: %ldms\n", duration.count());
    printf("Norma p con TBB: %f\n", normaTBB);

    return 0;
}