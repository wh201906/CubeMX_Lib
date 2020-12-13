#ifndef _DIST_US_H
#define _DIST_US_H
#include "main.h"
#include "DELAY/delay.h"
#include "tim.h"

void Dist_US_Start(void); 
uint32_t Dist_US_GetDistI(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
float Dist_US_GetDistF(void);

//if the freq of TIMx is aMHz, the Prescaler should be set to (a-1), then the coefficient in HAL_TIM_IC_CaptureCallback() is 17
//for better precision, the Prescaler can be set to (a/2-1), then the coffecient should be 8.5

#endif