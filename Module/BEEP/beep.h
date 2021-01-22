#ifndef _BEEP_H
#define _BEEP_H

#include "main.h"
#include "DELAY/delay.h"
#include "tim.h"

void Beep_SetTIMPara(uint32_t arrVal, uint16_t pscVal, uint32_t cmpVal);
void Beep_SetTIMClkFreq(uint16_t freq);
void Beep_SetSYSClkFreq(uint16_t freq);
void Beep_Init_TIM(TIM_HandleTypeDef *TIMHandle, uint32_t TIMChannel, uint16_t TIMFreq);
void Beep_Init_Delay(uint16_t sysFreq);

#endif