#include <stdint.h>

typedef enum {
    PlusZero = 0x00,
    MinusZero = 0x01,
    PlusInf = 0xF0,
    MinusInf = 0xF1,
    PlusRegular = 0x10,
    MinusRegular = 0x11,
    PlusDenormal = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN = 0x30,
    QuitNaN = 0x31
} float_class_t;

typedef union {
    double double_val;
    struct {
        uint64_t mantissa_val : 52;
        uint64_t exp_val : 11;
        uint64_t sign_val : 1;
    };
} double_parser_t;

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

uint64_t isAllOnes(uint64_t n)
{
    n = ~n;
    return isAllZeros(n);
};

extern float_class_t classify(double* val)
{
    const uint64_t expMask = ~(0b011111111111ull << 52);
    double_parser_t x = {*val};
    // whether exponent is 111...11
    uint64_t isSpecial = isAllOnes(((uint64_t)(x.exp_val) << 52) | expMask);
    uint64_t answer = isSpecial;
    answer |= answer << 1;

    // the value of mantissa highest bit
    uint64_t isQuite = (x.mantissa_val >> 51);

    // whether exponent is 000..00
    uint64_t isZeroMantissa = isAllZeros((uint64_t)(x.mantissa_val));
    uint64_t isInf = isSpecial & isZeroMantissa;

    uint64_t lowestBit;
    // case 1: the number is infinity, therefore we have to consider the sign
    lowestBit = x.sign_val & isInf;
    // case 2: the number is nan, therefore we have to consider quietness
    lowestBit |= isQuite & isSpecial & (isZeroMantissa ^ 1);
    // case 3: the number is regular, we have to consider the sign
    lowestBit |= (x.sign_val & (isSpecial ^ 1));
    // these 3 cases are mutually exclusive and one of them will definitely happen

    uint64_t isZeroExp = isAllZeros((uint64_t)(x.exp_val));
    // if the exponent is zero and mantissa is not zero then the number is denormalized
    answer |= isZeroExp & (isZeroMantissa ^ 1);
    // if the exponent is neither 111..11 nor 000..00 then the number is regular non-zero
    answer |= (isSpecial ^ 1) & (isZeroExp ^ 1);

    // at this point answer is 0 if the number is zero and 1 otherwise
    answer |= answer << isInf;
    answer |= answer << isInf;
    answer <<= 4;

    answer <<= isZeroExp;
    return answer | lowestBit;
}
