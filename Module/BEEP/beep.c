#include "beep.h"

TIM_HandleTypeDef *BeepTIMHandle = &htim3;
uint32_t BeepTIMChannel = TIM_CHANNEL_1;
uint16_t BeepTIMClkFreq;
uint16_t BeepSYSClkFreq;
uint8_t BeepMode = 0; // 0:TIM and PWM, 1:Delay

float freqTable[1][1];

void Beep_SetTIMPara(uint32_t arrVal, uint16_t pscVal, uint32_t cmpVal)
{
  __HAL_TIM_SET_AUTORELOAD(BeepTIMHandle, arrVal);
  __HAL_TIM_SET_PRESCALER(BeepTIMHandle, pscVal);
  __HAL_TIM_SET_COMPARE(BeepTIMHandle, BeepTIMChannel, cmpVal);
}

void Beep_SetTIMClkFreq(uint16_t freq)
{
  BeepTIMClkFreq = freq;
}

void Beep_SetSYSClkFreq(uint16_t freq)
{
  BeepSYSClkFreq = freq;
}

void Beep_Init_TIM(TIM_HandleTypeDef *TIMHandle, uint32_t TIMChannel, uint16_t TIMFreq)
{
  BeepTIMHandle = TIMHandle;
  BeepTIMChannel = TIMChannel;
  Beep_SetTIMClkFreq(TIMFreq);
  BeepMode = 0;
}

void Beep_Init_Delay(uint16_t sysFreq)
{
  Beep_SetSYSClkFreq(sysFreq);
  BeepMode = 1;
}