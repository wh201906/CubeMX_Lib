#ifndef _MLX90614_H
#define _MLX90614_H

#define MLX90614_ADDR 0x5A

#include "main.h"
#include "I2C/softi2c2.h"
#include "DELAY/delay.h"
#include "I2C/crc8.h"

void MLX90614_Init(void);
uint8_t MLX90614_WriteROM(uint16_t deviceAddr, uint8_t romAddr, uint16_t data);
uint8_t MLX90614_ReadReg(uint16_t deviceAddr, uint8_t regAddr, uint16_t *data);
uint8_t MLX90614_ReadRAM(uint16_t deviceAddr, uint8_t ramAddr, uint16_t *data);
uint8_t MLX90614_ReadROM(uint16_t deviceAddr, uint8_t romAddr, uint16_t *data);
float MLX90614_GetTemp(uint8_t reg);

#endif