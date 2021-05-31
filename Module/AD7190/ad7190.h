#ifndef _AD7190_H
#define _AD7190_H

#include "main.h"
#include "DELAY/delay.h"

#define AD7190_CS_PIN GPIO_PIN_0
#define AD7190_SCK_PIN GPIO_PIN_1
#define AD7190_DIN_PIN GPIO_PIN_3  // DIN: STM32->AD7190
#define AD7190_DOUT_PIN GPIO_PIN_2 // DIN: AD7190->STM32

#define AD7190_CS_GPIO GPIOC
#define AD7190_SCK_GPIO GPIOC
#define AD7190_DIN_GPIO GPIOC
#define AD7190_DOUT_GPIO GPIOC

#define AD7190_CS_CLKEN() __HAL_RCC_GPIOC_CLK_ENABLE()
#define AD7190_SCK_CLKEN() __HAL_RCC_GPIOC_CLK_ENABLE()
#define AD7190_DIN_CLKEN() __HAL_RCC_GPIOC_CLK_ENABLE()
#define AD7190_DOUT_CLKEN() __HAL_RCC_GPIOC_CLK_ENABLE()

uint8_t AD7190_Init(void);
uint8_t AD7190_GetID(void);
void AD7190_Reset(void);

uint8_t AD7190_Write(uint32_t data, uint8_t bitLen);
uint32_t AD7190_Read(uint8_t bitLen);

#endif