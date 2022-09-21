#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define ITYPE uint32_t

void sum(ITYPE first, ITYPE second, ITYPE* res, int* CF)
{
  *res = 0;
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

int main() {
  ITYPE* res = malloc(sizeof(ITYPE));
  int* cf = malloc(sizeof(int));
  ITYPE first = 5;
  ITYPE second = 2;
  mul(first, second, res, cf);
  printf("%u %i\n", *res, *cf);
}