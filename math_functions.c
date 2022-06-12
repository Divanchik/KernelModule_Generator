#include <stdio.h>
#include <stdlib.h>

float absf(float x)
{
    return x < 0 ? -x : x;
}

float powf(float x, int n)
{
    float res = 1;
    while (n > 0)
    {
        res *= x;
        n--;
    }
    return res;
}

float logf(float x)
{
    int n = 1;
    float x0 = 0, x1 = 0;
    while (1)
    {
        if (n % 2 == 1)
            x1 += powf(x-1, n)/n;
        else
            x1 -= powf(x-1, n)/n;
        if (absf(x1-x0) < 0.000001)
            break;
        x0 = x1;
        n++;
    }
    return x1;
}

float sqrtf(float a)
{
    float x0 = a/2;
    float x1 = 0;
    while (1)
    {
        x1 = (a/x0 + x0)/2;
        if (absf(x1 - x0) < 0.000001)
            break;
        x0 = x1;
    }
    return x1;
}

int main(int argc, char **argv)
{
    printf("sqrtf(%f) = %f\n", atof(argv[1]), sqrtf(atof(argv[1])));
    printf("logf(%f) = %f\n", atof(argv[1]), logf(atof(argv[1])));
    return 0;
}
