#ifndef _GRIDKEY_GPIO_H
#define _GRIDKEY_GPIO_H
#include "main.h"

// pin[3:0] should be configured as output
// pin[7:4] should be configured as input and be pulled down

#ifdef STM32H750xx
GPIO_TypeDef* const grp[8]={GPIOC,GPIOB,GPIOE,GPIOE,GPIOE,GPIOE,GPIOE,GPIOB};
const uint16_t pin[8]={GPIO_PIN_5,GPIO_PIN_1,GPIO_PIN_7,GPIO_PIN_9,GPIO_PIN_11,GPIO_PIN_13,GPIO_PIN_15,GPIO_PIN_11};
#endif

#ifdef STM32F407xx
GPIO_TypeDef* const grp[8]={GPIOB,GPIOE,GPIOE,GPIOE,GPIOE,GPIOB,GPIOB,GPIOB};
const uint16_t pin[8]={GPIO_PIN_1,GPIO_PIN_8,GPIO_PIN_10,GPIO_PIN_12,GPIO_PIN_14,GPIO_PIN_10,GPIO_PIN_12,GPIO_PIN_14};
#endif

#define SET GPIO_PIN_SET
#define RESET GPIO_PIN_RESET
const GPIO_PinState probeState[5][4]=
{
    SET,RESET,RESET,RESET,
    RESET,SET,RESET,RESET,
    RESET,RESET,SET,RESET,
    RESET,RESET,RESET,SET,
    SET,SET,SET,SET
};
#undef SET
#undef RESET

const uint8_t resultMap[2][16]=
{
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
    15,11,7,3,14,10,6,2,13,9,5,1,12,8,4,0
};

#endif
// used to make grp[], pin[] private
