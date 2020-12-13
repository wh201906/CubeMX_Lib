#ifndef _DIST_US_H
#define _DIST_US_H
#include "main.h"
#include "DELAY/delay.h"

void Dist_US_Init(uint8_t SYSCLK);
void Dist_US_Start(void);
uint32_t Dist_US_GetDistI(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
float Dist_US_GetDistF(void);

#endif