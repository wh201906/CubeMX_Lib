#ifndef _AD7190_H
#define _AD7190_H

#include "main.h"
#include "DELAY/delay.h"

#define AD7190_CS_PIN GPIO_PIN_10
#define AD7190_SCK_PIN GPIO_PIN_11
#define AD7190_DIN_PIN GPIO_PIN_12  // DIN: STM32->AD7190
#define AD7190_DOUT_PIN GPIO_PIN_15 // DIN: AD7190->STM32

#define AD7190_CS_GPIO GPIOB
#define AD7190_SCK_GPIO GPIOB
#define AD7190_DIN_GPIO GPIOB
#define AD7190_DOUT_GPIO GPIOE

#define AD7190_CS_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define AD7190_SCK_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define AD7190_DIN_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define AD7190_DOUT_CLKEN() __HAL_RCC_GPIOE_CLK_ENABLE()

uint8_t AD7190_Init(void);
uint8_t AD7190_GetID(void);
void AD7190_Reset(void);

uint8_t AD7190_Write(uint32_t data, uint8_t bitLen);
uint32_t AD7190_Read(uint8_t len);
void AD7190_SCKPulse(void);

#endif