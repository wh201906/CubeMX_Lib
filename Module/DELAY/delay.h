#ifndef _DELAY_H
#define _DELAY_H
#include "main.h" 

void Delay_Init(uint16_t SYSCLK);
void Delay_ms(uint16_t nms);
void Delay_us(uint32_t nus);
#endif

