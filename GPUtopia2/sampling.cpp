#include "sampling.h"

// will return F_N
int fibonacci_number(const int N)
{
    int a = 0;
    int b = 0;
    int c = 1;
    for (int ii = 0; ii < N; ii++)
    {
        a = b;
        b = c;
        c = a + b;
    }
    return c;
}