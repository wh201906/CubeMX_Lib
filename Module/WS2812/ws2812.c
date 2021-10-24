#include "ws2812.h"
#include "tim.h"

DMA_HandleTypeDef myhdma;
TIM_HandleTypeDef* myhtim;
uint32_t pwmBuf[8];
uint32_t counter = 0;
uint8_t* dataBuf;
uint32_t dataLen;

void DMAInit()
{
  __HAL_RCC_DMA2_CLK_ENABLE();
  
  myhdma.Instance = DMA2_Stream5;
  myhdma.Init.Channel = DMA_CHANNEL_6;
  myhdma.Init.Direction = DMA_MEMORY_TO_PERIPH;
  myhdma.Init.PeriphInc = DMA_PINC_DISABLE;
  myhdma.Init.MemInc = DMA_MINC_ENABLE;
  myhdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  myhdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  myhdma.Init.Mode = DMA_CIRCULAR;
  myhdma.Init.Priority = DMA_PRIORITY_VERY_HIGH;
  myhdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  
  HAL_DMA_DeInit(&myhdma);
  HAL_DMA_Init(&myhdma);

  __HAL_LINKDMA(myhtim,hdma[TIM_DMA_ID_UPDATE],myhdma);
  
  HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);

  __HAL_DMA_CLEAR_FLAG(&myhdma, __HAL_DMA_GET_TC_FLAG_INDEX(&myhdma));
  __HAL_DMA_CLEAR_FLAG(&myhdma, __HAL_DMA_GET_HT_FLAG_INDEX(&myhdma));
  myhdma.Instance->CR |= DMA_IT_TC | DMA_IT_HT;
}

void Test1(uint8_t* data, uint32_t len)
{
  uint8_t i;
  dataBuf = data;
  dataLen = len;
  counter = 0;
  for(i = 0; i < 8; i++)
    pwmBuf[i] = 0;
  myhtim = &htim1;
  DMAInit();
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_DMA_Init(&myhdma); // necessary there
  __HAL_TIM_ENABLE_DMA(myhtim, TIM_DMA_UPDATE);
  HAL_DMA_Start(&myhdma, (uint32_t)pwmBuf, (uint32_t) & (myhtim->Instance->CCR1), 8);
}

void WS2812_UpdateBuf(void)
{
  uint8_t i = 4;
    if(counter >= dataLen * 8 + 4)
    __HAL_DMA_DISABLE(&myhdma);
    __HAL_TIM_SET_COMPARE(myhtim, TIM_CHANNEL_1, 0);
  while(i--)
  {
    pwmBuf[counter % 8] = dataBuf[counter / 8] << (counter % 8) & 0x80 ? 143 : 67;
    counter++;
  }
}

void DMA2_Stream5_IRQHandler(void)
{
  if(__HAL_DMA_GET_FLAG(&myhdma, __HAL_DMA_GET_HT_FLAG_INDEX(&myhdma)))
  {
    __HAL_DMA_CLEAR_FLAG(&myhdma, __HAL_DMA_GET_HT_FLAG_INDEX(&myhdma));
    WS2812_UpdateBuf();
  }
  else if(__HAL_DMA_GET_FLAG(&myhdma, __HAL_DMA_GET_TC_FLAG_INDEX(&myhdma)))
  {
    __HAL_DMA_CLEAR_FLAG(&myhdma, __HAL_DMA_GET_TC_FLAG_INDEX(&myhdma));
    WS2812_UpdateBuf();
  }
}