#ifndef _AD9833_H
#define _AD9833_H

#include "main.h"
#include "spi.h"

#define POW2_28 268435456u
#define AD9833_CLK 25000000u
#define RESET 0

#define SINWAVE 1
#define TRIWAVE 2
#define SQWAVE 3

#define FREQREG_ALL 4
#define FREQREG_MSB 5
#define FREQREG_LSB 6

#define MCLK 7

void AD9833_SendRaw(uint16_t data);
uint32_t AD9833_GetFReg(double freq, uint8_t regID);

#endif