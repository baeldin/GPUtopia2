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

bool is_fibonacci_number(const uint32_t N)
{
    uint32_t checkNum = 0;
    uint32_t idx = 0;
    while (checkNum <= N)
    {
        if (fibonacci_number(idx) == N)
        {
            return true;
        }
        idx++;
    }
    return false;
}