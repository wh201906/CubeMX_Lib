#ifndef _SI4703_H
#define _SI4703_H

#include "main.h"

#define SI4703_ADDR 0x10

uint32_t SI4703_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID);
uint8_t SI4703_GetReg(uint8_t reg);
uint8_t SI4703_WriteReg(uint8_t reg, uint16_t data);
void SI4703_Reset(void);
uint32_t SI4703_ReadID(void); // DeviceID+ChipID
uint8_t SI4703_SetFreq(double freq);
uint8_t SI4703_SetVolume(uint8_t volume);

#endif