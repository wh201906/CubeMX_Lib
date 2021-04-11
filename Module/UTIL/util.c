#include "util.h"

char radixTable[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// this function will not return the str address,
// and doesn't support str=nullptr
// the return value is the length of str(without '\0')
uint16_t myitoa(int64_t val, char *str, uint8_t radix)
{
  // i: the index
  // n: the length(like strlen())

  // the n will not exceed 66(binrary, with '-' and '\0'), so uint16_t is enough.
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
  val = (val < 0 ? part - val : val - part);
  for (int i = 0; i < MYUTIL_FLOAT_PRECISION; i++) // print decimal part from left to right
  {
    val *= 10;
    str[len++] = (uint64_t)val % 10 + '0';
  }
  str[len] = '\0';
  while (str[--len] == '0' && str[len - 1] != '.') // remove trailing zeros
    str[len] = '\0';
  len++;
  return len;
}

// no overflow detect, skipping leading spaces
int64_t myatoi(char *str)
{
  int64_t val = 0;
  uint8_t i = 0, isPositive = 1;

  while (str[i] == ' ') // skip leading spaces
    i++;

  isPositive = (str[i] != '-'); // handle sign
  if (str[i] == '+' || str[i] == '-')
    i++;

  while (str[i] >= '0' && str[i] <= '9') // read until isdigit(str[i])=false, this will also handle '\0'
  {
    val *= 10;
    val += str[i++] - '0';
  }

  if (!isPositive) // handle negative number
    val = -val;
  return val;
}

double myatof(char *str)
{
  double val = 0, floatPart = 0;
  uint16_t dotPos = 0, len = 0;

  val = myatoi(str); // get int part

  while (str[len] != '\0') // get string length
    len++;
  for (; dotPos < len; dotPos++) // get dot position
    if (str[dotPos] == '.')
      break;

  if (dotPos + 1 >= len) // no float part detected, treat as myatoi with double output
    return val;

  floatPart = myatoi(str + dotPos + 1);
  while (dotPos + 1 < len)
  {
    floatPart /= 10.0;
    dotPos++;
  }
  val += val < 0 ? -floatPart : floatPart;
  return val;
}
