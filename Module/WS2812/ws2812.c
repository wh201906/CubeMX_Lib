#include "ws2812.h"
#include "tim.h"

WS2812_Dev *WS2812_currDev;
uint32_t WS2812_counter;
uint32_t WS2812_buf[8];
uint32_t WS2812_currTIMChannel;

void WS2812_DMAInit(WS2812_Dev *dev, DMA_Stream_TypeDef *DMAStream, uint32_t DMAChannel, IRQn_Type DMAIRQ)
{
  if ((uint32_t)DMAStream > DMA2_BASE)
    __HAL_RCC_DMA2_CLK_ENABLE();
  else if ((uint32_t)DMAStream > DMA1_BASE)
    __HAL_RCC_DMA1_CLK_ENABLE();

  dev->DMAHandle.Instance = DMA2_Stream5;
  dev->DMAHandle.Init.Channel = DMA_CHANNEL_6;
  dev->DMAHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
  dev->DMAHandle.Init.PeriphInc = DMA_PINC_DISABLE;
  dev->DMAHandle.Init.MemInc = DMA_MINC_ENABLE;
  dev->DMAHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dev->DMAHandle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  dev->DMAHandle.Init.Mode = DMA_CIRCULAR;
  dev->DMAHandle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
  dev->DMAHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  HAL_DMA_DeInit(&dev->DMAHandle);
  HAL_DMA_Init(&dev->DMAHandle);

  __HAL_LINKDMA(dev->htim, hdma[TIM_DMA_ID_UPDATE], dev->DMAHandle);

  HAL_NVIC_SetPriority(DMAIRQ, 1, 0);
  HAL_NVIC_EnableIRQ(DMAIRQ);

  __HAL_DMA_CLEAR_FLAG(&dev->DMAHandle, __HAL_DMA_GET_TC_FLAG_INDEX(&dev->DMAHandle));
  __HAL_DMA_CLEAR_FLAG(&dev->DMAHandle, __HAL_DMA_GET_HT_FLAG_INDEX(&dev->DMAHandle));
  dev->DMAHandle.Instance->CR |= DMA_IT_TC | DMA_IT_HT;
}

void WS2812_Init(WS2812_Dev *dev, DMA_Stream_TypeDef *DMAStream, uint32_t DMAChannel, IRQn_Type DMAIRQ, TIM_HandleTypeDef *htim)
{
  uint32_t tmp;
  dev->htim = htim;

  // for PWM mode 1
  tmp = __HAL_TIM_GET_AUTORELOAD(htim);
  dev->comp0 = tmp * 0.32;
  dev->comp1 = tmp - dev->comp0;
  WS2812_DMAInit(dev, DMAStream, DMAChannel, DMAIRQ);
}

void WS2812_Write(WS2812_Dev *dev, uint32_t TIMChannel, uint8_t *data, uint32_t len)
{
  uint8_t i;
  WS2812_currDev = dev;
  WS2812_currDev->data = data;
  WS2812_currDev->len = len;
  WS2812_currTIMChannel = TIMChannel;
  WS2812_counter = 0;
  for (i = 0; i < 8; i++)
    WS2812_buf[i] = 0;
  __HAL_TIM_SET_COMPARE(WS2812_currDev->htim, TIMChannel, 0);
  WS2812_currDev->htim->Instance->EGR = TIM_EGR_UG; // force update
  __HAL_TIM_SET_COUNTER(WS2812_currDev->htim, 0);
  HAL_TIM_PWM_Start(WS2812_currDev->htim, TIMChannel);
  HAL_DMA_Init(&WS2812_currDev->DMAHandle); // necessary there
  __HAL_TIM_ENABLE_DMA(WS2812_currDev->htim, TIM_DMA_UPDATE);
  HAL_DMA_Start(&WS2812_currDev->DMAHandle, (uint32_t)WS2812_buf, (uint32_t) & (WS2812_currDev->htim->Instance->CCR1) + TIMChannel, 8);
  while (WS2812_currDev->DMAHandle.Instance->CR & DMA_SxCR_EN)
    ;
}

inline void WS2812_UpdateBuf(void)
{
  uint8_t i;
  if (WS2812_counter >= WS2812_currDev->len * 8 + 4) // boundary condition, modify this if necessary
  {
    __HAL_DMA_DISABLE(&WS2812_currDev->DMAHandle);
    // wait for the last bit
    while(__HAL_TIM_GET_COUNTER(WS2812_currDev->htim) >= __HAL_TIM_GET_COMPARE(WS2812_currDev->htim, WS2812_currTIMChannel)) // boundary condition, modify this if necessary
      ;
    __HAL_TIM_SET_COMPARE(WS2812_currDev->htim, WS2812_currTIMChannel, 0);
  }
  i = 4;
  while (i--)
  {
    WS2812_buf[WS2812_counter % 8] = WS2812_currDev->data[WS2812_counter / 8] << (WS2812_counter % 8) & 0x80 ? WS2812_currDev->comp1 : WS2812_currDev->comp0;
    WS2812_counter++;
  }
  __HAL_DMA_CLEAR_FLAG(&WS2812_currDev->DMAHandle, __HAL_DMA_GET_HT_FLAG_INDEX(&WS2812_currDev->DMAHandle) | __HAL_DMA_GET_TC_FLAG_INDEX(&WS2812_currDev->DMAHandle));
}