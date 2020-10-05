#ifndef _DELAY_H
#define _DELAY_H
#include "main.h" 

void delay_init(uint16_t SYSCLK);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);
#endif

