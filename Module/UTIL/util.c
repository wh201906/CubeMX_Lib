#include "util.h"

char radixTable[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// this function will not return the str address,
// and doesn't support str=nullptr
// the return value is the length of str(without '\0')
uint16_t myitoa(int64_t val, char *str, uint8_t radix)
{
    // i: the index
    // n: the length(like strlen())

    // the n will not exceed 65, so uint16_t is enough.
    uint16_t i = 0, n = 0;
    uint8_t isPositive = 1;
    if (radix < 2 || radix > 36)
        return 0;
    if (val < 0)
    {
        val = -val;
        isPositive = 0;
    }
    do
    {
        str[n++] = radixTable[val % radix];
        val /= radix;
    } while (val > 0);
    if (!isPositive)
        str[n++] = '-';
    for (i = 0; i < n / 2; i++) // reverse, use str[n] as tempVar
    {
        str[n] = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = str[n];
    }
    str[n] = '\0';
    return n;
}

// when the radix is 10, the len will not reach the maximum value of uint8_t.
uint8_t myftoa(double val, char *str)
{
    int64_t part = val;
    uint8_t len;
    len = myitoa(part, str, 10);
    str[len++] = '.';
    part = (val < 0 ? part - val : val - part) * 1000; // 3 digit of float part.
    len += myitoa(part, str + len, 10);
    return len;
}