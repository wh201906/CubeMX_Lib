#ifndef _RDA5820_H
#define _RDA5820_H

#include "main.h"
#include "I2C/softi2c.h"
#include "DELAY/delay.h"

#define RDA5820_ADDR 0x11

#define RDA5820_WORKMODE_RX 0x0
#define RDA5820_WORKMODE_TX 0x1
#define RDA5820_WORKMODE_AMP 0x8
#define RDA5820_WORKMODE_CODEC 0xC
#define RDA5820_WORKMODE_ADC 0xE

uint16_t RDA5820_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID);
uint8_t RDA5820_ReadReg(uint8_t reg, uint16_t *data);
uint8_t RDA5820_WriteReg(uint8_t reg, uint16_t data);
uint16_t RDA5820_ReadID(void);
uint8_t RDA5820_SetWorkMode(uint8_t workMode);
uint8_t RDA5820_SetFreq(double freq);      //50~115, in MHz, maximum precision
uint8_t RDA5820_SetVolume(uint8_t volume); // 4bit, 0~15
uint8_t RDA5820_test(int freq);

#endif