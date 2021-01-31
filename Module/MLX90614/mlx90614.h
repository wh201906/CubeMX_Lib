#ifndef _MLX90614_H
#define _MLX90614_H

#define MLX90614_IRRAW1 0X04
#define MLX90614_IRRAW2 0X05
#define MLX90614_TEMP_AMBIENT 0x06
#define MLX90614_TEMP_OBJECT 0x07
#define MLX90614_TEMP_OBJECT1 0x07
#define MLX90614_TEMP_OBJECT2 0x08

#define MLX90614_TOMAX 0x00
#define MLX90614_TOMIN 0x01
#define MLX90614_PWMCTRL 0x02
#define MLX90614_TA_RANGE 0x03
#define MLX90614_EMISSIVITY 0x04
#define MLX90614_CONFREG1 0x05
#define MLX90614_ADDRREG 0x0E
#define MLX90614_IDREG1 0x1C
#define MLX90614_IDREG2 0x1D
#define MLX90614_IDREG3 0x1E
#define MLX90614_IDREG4 0x1F

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
void MLX90614_Sleep(uint8_t enterSleep); // 1:sleep 0:wakeup
uint8_t MLX90614_SetI2CAddr(uint8_t addr, uint8_t changeAddrReg); // 0:just change the address in program 1:also change the address of device

#endif