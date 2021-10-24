#ifndef _WS2812_H
#define _WS2812_H

#include "main.h"

typedef struct _WS2812_Dev
{
  DMA_HandleTypeDef DMAHandle;
  TIM_HandleTypeDef *htim;
  uint8_t *data;
  uint32_t len;

  uint32_t comp0;
  uint32_t comp1;
} WS2812_Dev;

void WS2812_DMAInit(WS2812_Dev *dev, DMA_Stream_TypeDef *DMAStream, uint32_t DMAChannel, IRQn_Type DMAIRQ);
void WS2812_Init(WS2812_Dev *dev, DMA_Stream_TypeDef *DMAStream, uint32_t DMAChannel, IRQn_Type DMAIRQ, TIM_HandleTypeDef *htim);
void WS2812_Write(WS2812_Dev *dev, uint32_t TIMChannel, uint8_t *data, uint32_t len);
void WS2812_UpdateBuf(void);

void Test1(uint8_t *data, uint32_t len);

#endif