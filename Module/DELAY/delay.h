#ifndef _DELAY_H
#define _DELAY_H
#include "main.h" 

void Delay_Init(uint16_t SYSCLK);
void Delay_ms(uint16_t nms);
void Delay_us(uint32_t nus);
void Delay_ticks(uint32_t ticks);
uint32_t Delay_GetSYSFreq(void);

#endif

