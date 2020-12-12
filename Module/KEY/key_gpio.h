#ifndef _KEY_GPIO_H
#define _KEY_GPIO_H
#include "main.h"

#ifdef STM32H750xx
#define KEY_NUM 1
const GPIO_TypeDef *grp[KEY_NUM] = {GPIOC};
const uint16_t pin[KEY_NUM] = {GPIO_PIN_13};
const GPIO_PinState downSt[KEY_NUM] = {GPIO_PIN_SET};
#endif

#ifdef STM32F407xx
#define KEY_NUM 3
const GPIO_TypeDef *grp[KEY_NUM] = {GPIOE, GPIOE, GPIOA};
const uint16_t pin[KEY_NUM] = {GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_0};
const GPIO_PinState downSt[KEY_NUM] = {GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET};
#endif

#endif
// used to make grp[], pin[] and downSt[] private
