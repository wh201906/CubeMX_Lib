#ifndef _SI4703_H
#define _SI4703_H

#include "main.h"

#define SI4703_ADDR 0x10

#define SI4703_DEVICEID 0x00
#define SI4703_CHIPID 0x01
#define SI4703_PWR 0x02
#define SI4703_CH 0x03
#define SI4703_CONF1 0x04
#define SI4703_CONF2 0x05
#define SI4703_CONF3 0x06
#define SI4703_TEST1 0x07
#define SI4703_TEST2 0x08
#define SI4703_BOOT 0x09
#define SI4703_STATUS 0x0A
#define SI4703_READCH 0x0B
#define SI4703_RDSA 0x0C
#define SI4703_RDSB 0x0D
#define SI4703_RDSC 0x0E
#define SI4703_RDAD 0x0F

#define SI4703_READALL 0x09
#define SI4703_WRITEALL 0x07

uint32_t SI4703_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID);
uint8_t SI4703_GetReg(uint8_t reg);
uint8_t SI4703_SetReg(uint8_t reg);
void SI4703_Reset(void);
uint32_t SI4703_ReadID(void); // DeviceID+ChipID
uint8_t SI4703_SetFreq(double freq);
uint8_t SI4703_SetVolume(uint8_t volume);

#endif