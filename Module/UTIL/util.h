#ifndef _MY_UTIL_H
#define _MY_UTIL_H

#include "main.h"

#define MYUTIL_FLOAT_PRECISION 6

uint16_t myitoa(int64_t val, char *str, uint8_t radix);
uint8_t myftoa_FD(double val, char *str, uint8_t precision);
uint8_t myftoa(double val, char *str);
int64_t myatoi(char *str);
int64_t myatoi_hex(char *str);
double myatof(char *str);
int64_t mygcd(int64_t a, int64_t b);

#endif