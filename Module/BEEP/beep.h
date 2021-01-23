#ifndef _BEEP_H
#define _BEEP_H

#include "main.h"
#include "DELAY/delay.h"
#include "tim.h"

// Csharp: C#
// Cflat: Cb
#define NOTE_C 0
#define NOTE_Csharp 1
#define NOTE_Dflat 1
#define NOTE_D 2
#define NOTE_Dsharp 3
#define NOTE_Eflat 3
#define NOTE_E 4
#define NOTE_F 5
#define NOTE_Fsharp 6
#define NOTE_Gflat 6
#define NOTE_G 7
#define NOTE_Gsharp 8
#define NOTE_Aflat 8
#define NOTE_A 9
#define NOTE_Asharp 10
#define NOTE_Bflat 10
#define NOTE_B 11

void Beep_SetTIMPara(uint32_t arrVal, uint16_t pscVal, uint32_t cmpVal);
void Beep_SetTIMClkFreq(uint16_t freq);
void Beep_SetSYSClkFreq(uint16_t freq);
void Beep_Init_TIM(TIM_HandleTypeDef *TIMHandle, uint32_t TIMChannel, uint16_t TIMFreq);
void Beep_Init_Delay(uint16_t sysFreq);

// TIM&PWM mode: use PWM to generate the tune, use DELAY/delay.c to play for a while
// Delay mode: use internal delay(based on SysTick) to generate the tune and play for a while

#endif