#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>

#include "cephes.h"

#define ALPHA 0.01

void Frequency(int *a, size_t n)
{
    int sum = 0;
    for (size_t i = 0; i < n; i++)
        sum += a[i] * 2 - 1;
    double s_obs = fabs(sum) / sqrt(n);
    double p_value = erfc(s_obs / sqrt(2));
    std::cout << "Frequency test" << std::endl << "P_value = " << p_value << std::endl;
    std::cout << (p_value < ALPHA ? "FAILURE" : "SUCCESS") << std::endl;
}

int main()
{
    // подготовка
    srand(time(NULL));
    int n = 100000;
    int *a = new int[n];
    for (size_t i = 0; i < n; i++)
        a[i] = rand() % 2;

    Frequency(a, n);
    delete[] a;
    return 0;
}
