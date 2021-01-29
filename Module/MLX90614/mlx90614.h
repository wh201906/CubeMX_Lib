#ifndef _MLX90614_H
#define _MLX90614_H

#define MLX90614_ADDR 0x5A

#include "main.h"
#include "I2C/softi2c2.h"
#include "DELAY/delay.h"

void MLX90614_Init(void);
uint8_t MLX90614_WriteReg(uint16_t deviceAddr, uint8_t regAddr, uint16_t data);
uint8_t MLX90614_ReadReg(uint16_t deviceAddr, uint8_t regAddr, uint16_t *data);

#endif