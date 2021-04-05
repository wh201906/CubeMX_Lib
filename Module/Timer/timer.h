#ifndef _TIMER_H
#define _TIMER_H

#include "main.h"

// For STM32F4:
// Use TIM2/TIM5(32-bit, 84MHz max) for lower interrupt times
// Use TIM1/TIM8,TIM9~TIM11(16-bit, 168MHz max) for better precision

#define TIMER_TIM_CLKEN() __HAL_RCC_TIM9_CLK_ENABLE()
#define TIMER_TIM TIM9
#define TIMER_IRQN TIM1_BRK_TIM9_IRQn
// Remember to change TIMx_IRQHandler()

void Timer_Init(void);
void Timer_Start(void);
uint64_t Timer_Stop(void);

#endif