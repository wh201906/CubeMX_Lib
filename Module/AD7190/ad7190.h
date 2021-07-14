#ifndef _AD7190_H
#define _AD7190_H

#include "main.h"
#include "DELAY/delay.h"

#define AD7190_CS_PIN GPIO_PIN_0
#define AD7190_SCK_PIN GPIO_PIN_1
#define AD7190_DIN_PIN GPIO_PIN_3  // DIN: STM32->AD7190
#define AD7190_DOUT_PIN GPIO_PIN_2 // DOUT: AD7190->STM32

#define AD7190_CS_GPIO GPIOC
#define AD7190_SCK_GPIO GPIOC
#define AD7190_DIN_GPIO GPIOC
#define AD7190_DOUT_GPIO GPIOC

#define AD7190_CS_CLKEN() __HAL_RCC_GPIOC_CLK_ENABLE()
#define AD7190_SCK_CLKEN() __HAL_RCC_GPIOC_CLK_ENABLE()
#define AD7190_DIN_CLKEN() __HAL_RCC_GPIOC_CLK_ENABLE()
#define AD7190_DOUT_CLKEN() __HAL_RCC_GPIOC_CLK_ENABLE()

#define AD7190_POLAR_UNIPOLAR 0x8
#define AD7190_POLAR_BIPOLAR 0x00
#define AD7190_POLAR_MASK AD7190_POLAR_UNIPOLAR

#define AD7190_GAIN_1 0x0
#define AD7190_GAIN_8 0x3
#define AD7190_GAIN_16 0x4
#define AD7190_GAIN_32 0x5
#define AD7190_GAIN_64 0x6
#define AD7190_GAIN_128 0x7
#define AD7190_GAIN_MASK AD7190_GAIN_128

uint8_t AD7190_Init(double refV);
uint8_t AD7190_GetState(void);
uint8_t AD7190_GetID(void);
uint32_t AD7190_GetMode(void);
uint8_t AD7190_SetMode(uint32_t mode);
uint32_t AD7190_GetConf(void);
uint8_t AD7190_SetConf(uint32_t conf);
uint32_t AD7190_GetData(void);
uint32_t AD7190_GetDataWithState(void);
uint8_t AD7190_GetGPOCON(void);
uint8_t AD7190_SetGPOCON(uint8_t GPOCON);
void AD7190_Reset(void);

double AD7190_GetVoltage(void);
void AD7190_SetPolar(uint8_t polar);
void AD7190_SetGain(uint8_t gain);

void AD7190_UpdateConf(uint32_t conf);

uint8_t AD7190_Write(uint32_t data, uint8_t bitLen);
uint32_t AD7190_Read(uint8_t bitLen);

#endif