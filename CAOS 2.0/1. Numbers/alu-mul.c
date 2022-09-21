#include <limits.h>
#include <stdint.h>

void sum(ITYPE first, ITYPE second, ITYPE* res, int* CF)
{
    *CF = 0;
    for (uint32_t bit = 0; bit < sizeof(ITYPE) * CHAR_BIT; ++bit) {
        ITYPE sum = (first & 1) + (second & 1) + *CF;
        *CF = (sum > 1);
        *res |= ((sum & 1) << bit);
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
        sum(((first & 1) * second) << bit, *res, res, CF);
        *CF |= overflow;
        first >>= 1;
    }
}
