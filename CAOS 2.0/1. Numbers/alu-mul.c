#include <limits.h>
#include <stdint.h>

void sum(ITYPE first, ITYPE second, ITYPE* res, int* CF)
{
    *res = 0;
    *CF = 0;
    for (uint32_t bit = 0; bit < sizeof(ITYPE) * CHAR_BIT; ++bit) {
        ITYPE left = first & 1;
        ITYPE right = second & 1;
        *res |= (left ^ right ^ *CF) << bit;
        *CF = (int)((left & right) | (left & *CF) | (right & *CF));
        first >>= 1;
        second >>= 1;
    }
}

void mul(ITYPE first, ITYPE second, ITYPE* res, int* CF)
{
    *res = 0;
    *CF = 0;
    for (uint32_t bit = 0; bit < sizeof(ITYPE) * CHAR_BIT; ++bit) {
        int overflow = *CF;
        ITYPE mask = ~(ITYPE)(0);
        sum(mask, first & 1, &mask, CF);
        sum((~mask & second) << bit, *res, res, CF);
        *CF |= overflow;
        first >>= 1;
    }
}
