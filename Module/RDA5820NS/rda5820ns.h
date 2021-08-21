#ifndef _RDA5820_H
#define _RDA5820_H

#include "main.h"
#include "I2C/softi2c.h"
#include "DELAY/delay.h"

#define RDA5820_ADDR 0x11

void RDA5820_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID);
uint8_t RDA5820_ReadReg(uint8_t reg, uint16_t *data);
uint8_t RDA5820_WriteReg(uint8_t reg, uint16_t data);
uint16_t RDA5820_ReadID(void);

#endif