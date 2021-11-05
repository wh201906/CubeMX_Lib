#ifndef _TM1637_H
#define _TM1637_H

#include "main.h"
#include "I2C/softi2c.h"
#include "DELAY/delay.h"

void TM1637_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID);
uint8_t TM1637_Write(uint8_t *data, uint16_t len);
void TM1637_Test();

#endif