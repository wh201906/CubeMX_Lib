#ifndef _MOD_H
#define _MOD_H

#include "main.h"

#define SIGPARA_HTIM1_CLK 84000000.0

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

static void SigPara_Freq_LF_GPIO_Init(void);
static void SigPara_PWM_TIM_Init(void);
static void SigPara_PWM_GPIO_Init(void);
void SigPara_PWM_Init(void);
void SigPara_PWM();

#endif