#include "ws2812.h"
#include "tim.h"

DMA_HandleTypeDef myhdma;
TIM_HandleTypeDef* myhtim;
uint16_t pwmbuf[72]; // for ws2812, uint16_t is enough even the timer is 32-bit

void DMAInit()
{
  __HAL_RCC_DMA2_CLK_ENABLE();
  
  myhdma.Instance = DMA2_Stream5;
  myhdma.Init.Channel = DMA_CHANNEL_6;
  myhdma.Init.Direction = DMA_MEMORY_TO_PERIPH;
  myhdma.Init.PeriphInc = DMA_PINC_DISABLE;
  myhdma.Init.MemInc = DMA_MINC_ENABLE;
  myhdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  myhdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  myhdma.Init.Mode = DMA_NORMAL;
  myhdma.Init.Priority = DMA_PRIORITY_VERY_HIGH;
  myhdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  
  HAL_DMA_DeInit(&myhdma);
  HAL_DMA_Init(&myhdma);

  __HAL_LINKDMA(myhtim,hdma[TIM_DMA_ID_UPDATE],myhdma);
}

void Test1()
{
  myhtim = &htim1;
  DMAInit();
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_DMA_Init(&myhdma); // necessary there
  __HAL_TIM_ENABLE_DMA(myhtim, TIM_DMA_UPDATE);
  HAL_DMA_Start(&myhdma, (uint32_t)pwmbuf, (uint32_t) & (myhtim->Instance->CCR1), 72);
  while(__HAL_DMA_GET_FLAG(&myhdma, DMA_FLAG_TCIF1_5) == DMA_FLAG_TCIF1_5)
    ;
  Delay_us(90); // necessary, otherwise the compare will not be set
  __HAL_TIM_SET_COMPARE(myhtim, TIM_CHANNEL_1, 0);
}