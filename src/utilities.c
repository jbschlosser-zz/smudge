#include "utilities.h"

int modulo(int n, int d)
{
    if(d < 0)
        d = -d;
    return ((n % d) + d) % d;
}
