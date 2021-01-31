#ifndef _CRC8INI2C_H
#define _CRC8INI2C_H

#include "main.h"

void CRC8_Init(uint8_t polynomial);
uint8_t CRC8_Calc(uint8_t initVal, uint8_t *data, uint32_t dataLen);
uint8_t CRC8_CalcByte(uint8_t initVal, uint8_t data);

#endif