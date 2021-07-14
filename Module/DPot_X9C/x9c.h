#ifndef _DPOT_X9C_H
#define _DPOT_X9C_H

#include "main.h"
#include "DELAY/delay.h"

#define DPOTX9C_CS_PIN GPIO_PIN_10
#define DPOTX9C_INC_PIN GPIO_PIN_11
#define DPOTX9C_UD_PIN GPIO_PIN_12

#define DPOTX9C_CS_GPIO GPIOB
#define DPOTX9C_INC_GPIO GPIOB
#define DPOTX9C_UD_GPIO GPIOB

#define DPOTX9C_CS_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define DPOTX9C_INC_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define DPOTX9C_UD_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()

void DPotX9C_Init(void);
void DPotX9C_Change(int8_t delta);
void DPotX9C_Store(void);
void DPotX9C_Return(void);

#endif