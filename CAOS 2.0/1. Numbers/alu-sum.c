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
    ITYPE left = first & 1;
    ITYPE right = second & 1;
    *res |= (left ^ right ^ *CF) << bit;
    *CF = (int)((left & right) | (left & *CF) | (right & *CF));
    first >>= 1;
    second >>= 1;
  }
}

int main() {
  ITYPE first = 13;
  ITYPE second = 15;
  ITYPE* res = malloc(sizeof(ITYPE));
  int* CF = malloc(sizeof(int));
  sum(first, second, res, CF);
  printf("%d %d", *res, *CF);
}