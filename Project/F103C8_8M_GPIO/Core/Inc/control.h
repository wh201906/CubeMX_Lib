#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "main.h"
#include <string.h>
#include <ctype.h>
#include "UTIL/util.h"
#include "USART/myusart1.h"

#define GPIO_LED GPIOB
#define GPIO_PIN_LED GPIO_PIN_2
#define GPIO_KEY GPIOA
#define GPIO_PIN_KEY GPIO_PIN_0

uint8_t CTRL_IsPinValid(GPIO_TypeDef *grp, uint32_t pin);
uint8_t CTRL_ReadCmd(char *str);
uint8_t CTRL_SendState(GPIO_TypeDef *grp, uint8_t pin);
uint8_t CTRL_WritePin(GPIO_TypeDef *grp, uint8_t pin, uint8_t state);
void CTRL_WriteGrp(GPIO_TypeDef *grp, uint8_t state);
void CTRL_WriteAll(uint8_t state);
void CTRL_LED(uint8_t state);
uint8_t CTRL_KEY(void);

#endif