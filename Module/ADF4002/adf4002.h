#ifndef _ADF4002_H
#define _ADF4002_H

#include "main.h"

#define ADF4002_LE_PIN GPIO_PIN_0
#define ADF4002_SCK_PIN GPIO_PIN_1
#define ADF4002_DATA_PIN GPIO_PIN_3 

#define ADF4002_LE_GPIO GPIOC
#define ADF4002_SCK_GPIO GPIOC
#define ADF4002_DATA_GPIO GPIOC

#define ADF4002_LE_CLKEN() __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADF4002_SCK_CLKEN() __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADF4002_DATA_CLKEN() __HAL_RCC_GPIOC_CLK_ENABLE()

void ADF4002_Init(void);
void ADF4002_Write(uint8_t regID, uint32_t data);

void Test(uint32_t val);

#endif