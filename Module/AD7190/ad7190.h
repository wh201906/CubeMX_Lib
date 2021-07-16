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

#define AD7190_POLAR_UNIPOLAR 0x000008
#define AD7190_POLAR_BIPOLAR 0x000000
#define AD7190_POLAR_MASK AD7190_POLAR_UNIPOLAR

#define AD7190_GAIN_1 0x000000
#define AD7190_GAIN_8 0x000003
#define AD7190_GAIN_16 0x000004
#define AD7190_GAIN_32 0x000005
#define AD7190_GAIN_64 0x000006
#define AD7190_GAIN_128 0x000007
#define AD7190_GAIN_MASK AD7190_GAIN_128

#define AD7190_CH_12 0x000100
#define AD7190_CH_34 0x000200
#define AD7190_CH_TEMP 0x000400
#define AD7190_CH_22 0x000800
#define AD7190_CH_1G 0x001000
#define AD7190_CH_2G 0x002000
#define AD7190_CH_3G 0x004000
#define AD7190_CH_4G 0x008000
#define AD7190_CH_MASK 0x00FF00

#define AD7190_FS_MASK 0x0003FF

#define AD7190_CLK_EXT_12 0x000000
#define AD7190_CLK_EXT_2 0x040000
#define AD7190_CLK_INT 0x080000
#define AD7190_CLK_INT_OUT 0x0C0000
#define AD7190_CLK_MASK AD7190_CLK_INT_OUT

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
void AD7190_SetPolar(uint32_t polar);
void AD7190_SetGain(uint32_t gain);
void AD7190_SetChannel(uint32_t channel);
void AD7190_SetCLKSource(uint32_t source);
void AD7190_SetFS(uint32_t FS);

void AD7190_UpdateConf(uint32_t conf);

uint8_t AD7190_Write(uint32_t data, uint8_t bitLen);
uint32_t AD7190_Read(uint8_t bitLen);

#endif