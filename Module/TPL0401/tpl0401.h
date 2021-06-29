#ifndef _TPL0401_H
#define _TPL0401_H

#include "main.h"
#include "DELAY/delay.h"
#include "I2C/softi2c.h"

#define TPL0401_ADDR_A_C 0x2E
#define TPL0401_ADDR_B 0x3E
#define TPL0401_REG 0x00

void TPL0401_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID, char type);
uint8_t TPL0401_WriteVal(uint8_t data);
uint8_t TPL0401_ReadVal(void); // the value should be in [0:127], otherwise the value is invalid(failed to read)

#endif