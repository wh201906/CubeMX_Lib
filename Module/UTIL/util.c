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

// Fixed decimal digits
uint8_t myftoa_FD(double val, char *str, uint8_t precision)
{
  int64_t part;
  uint8_t len, i;
  double round = 0.5;
  
  // handle negative number
  // if -1 < val < 0, myitoa() will consider it as a non-negative number(0 as integer part)
  if(val<0)
  {
    *(str++) = '-';
    val = -val;
  }
  // for rounding
  for (i = 0; i < precision; i++)
    round /= 10;
  val += round;

  // for integer part
  part = val;
  len = myitoa(part, str, 10);

  // for decimal part
  // The multiplication is only operated on the decimal part rather than the orignal val,
  // which will decrease the precision loss
  if (precision == 0)
  {
    str[len] = '\0';
    return len;
  }
  str[len++] = '.';
  val -= part;
  for (i = 0; i < precision; i++) // print decimal part from left to right
  {
    val *= 10;
    str[len++] = (uint64_t)val % 10 + '0';
  }
  str[len] = '\0';

  return len;
}

// when the radix is 10, the len will not reach the maximum value of uint8_t.
uint8_t myftoa(double val, char *str)
{
  uint8_t len;

  len = myftoa_FD(val, str, MYUTIL_FLOAT_PRECISION);

  // remove trailing zeros
  while (str[--len] == '0' && str[len - 1] != '.')
    str[len] = '\0';
  len++;
  return len;
}

// no overflow detect, skipping leading spaces
int64_t myatoi(const char *str)
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

int64_t myatoi_hex(const char *str)
{
  int64_t val = 0;
  uint8_t i = 0, isPositive = 1;

  while (str[i] == ' ') // skip leading spaces
    i++;

  isPositive = (str[i] != '-'); // handle sign
  if (str[i] == '+' || str[i] == '-')
    i++;
  if (str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X'))
    i += 2;

  while (str[i] >= '0' && str[i] <= '9' || str[i] >= 'a' && str[i] <= 'f' || str[i] >= 'A' && str[i] <= 'F') // read until isdigit(str[i])=false, this will also handle '\0'
  {
    val *= 16;
    if (str[i] >= '0' && str[i] <= '9')
      val += str[i++] - '0';
    else if (str[i] >= 'a' && str[i] <= 'f')
      val += str[i++] - 'a' + 10;
    else
      val += str[i++] - 'A' + 10;
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

  while ((str[len] >= '0' && str[len] <= '9') || str[len] == '.' || str[len] == '-') // get float string length
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

void splitparam_hex(char *str, char spliter, int64_t *result, uint64_t num)
{
  uint64_t i;
  char *ptr;
  ptr = str;
  for (i = 0; i < num - 1 && *ptr != '\0'; i++)
  {
    result[i] = myatoi_hex(ptr);
    while (*ptr != spliter && *ptr != '\0')
      ptr++;
  }
  if (*ptr == spliter)
    ptr++;
  result[i] = myatoi_hex(ptr);
}

void splitparam_f(char *str, char spliter, double *result, uint64_t num)
{
  uint64_t i;
  char *ptr;
  ptr = str;
  for (i = 0; i < num - 1 && *ptr != '\0'; i++)
  {
    result[i] = myatof(ptr);
    while (*ptr != spliter && *ptr != '\0')
      ptr++;
  }
  if (*ptr == spliter)
    ptr++;
  result[i] = myatof(ptr);
}

int64_t mygcd(int64_t a, int64_t b)
{
  int64_t tmp;
  while (b > 0)
  {
    tmp = a % b;
    a = b;
    b = tmp;
  }
  return a;
}