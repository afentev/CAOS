#include <stdint.h>
#include <stdio.h>

uint64_t isAllZeros(uint64_t n)
{
    n |= n >> 32;
    n |= n >> 16;
    n |= n >> 8;
    n |= n >> 4;
    n |= n >> 2;
    n |= n >> 1;
    return (n & 1) ^ 1;
}

int main()
{
    int chr;
    int ascii = 0;
    int nonascii = 0;
    int skip = 0;
    while ((chr = getchar()) ^ EOF) {
        uint32_t c = chr;
        if (skip) {
            if ((c >> 6) ^ 0b10) {
                printf("%d %d", ascii, nonascii - 1);
                return 1;
            }
            skip >>= 1;
            continue;
        }
        int isUnicode = c >> 7;
        if (isUnicode) {
            if ((c >> 6) ^ 0b11 | isAllZeros(c ^ 0xFF)) {
                printf("%d %d", ascii, nonascii);
                return 1;
            }
            int count = __builtin_clz((~c & 0xFFu) << 26);
            if (count > 2) {
                printf("%d %d", ascii, nonascii);
                return 1;
            }
            nonascii++;
            skip = 1 << count;
        } else {
            ascii++;
        }
    }
    if (skip ^ 0) {
        printf("%d %d", ascii, nonascii - 1);
        return 1;
    }
    printf("%d %d", ascii, nonascii);
    return 0;
}
