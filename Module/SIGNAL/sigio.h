#ifndef _SIGIO_H
#define _SIGIO_H

#include "main.h"

extern DMA_HandleTypeDef SigIO_DMA_ADC;
extern DMA_HandleTypeDef SigIO_DMA_DAC;

void SigIO_Init(TIM_HandleTypeDef *htim, ADC_HandleTypeDef *hadc);
void SigIO_DMA_Init(void);
void SigIO_IT_Init(void);
void SigIO_Start(void *bufAddr, uint32_t partLen);
  
#endif