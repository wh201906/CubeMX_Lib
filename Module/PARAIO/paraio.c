#include "paraio.h"

// on STM32F4, GPIO is on AHB1, which is only accessible by DMA2
// the Timer should be configured by CubeMX

DMA_HandleTypeDef ParaIO_DMA_In;
TIM_HandleTypeDef* ParaIO_TIM_In=&htim8;

void ParaIO_Init_GPIO_In(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  PARAIO_DATAIN_CLKEN();

  GPIO_InitStruct.Pin = 0xFFFF;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(PARAIO_DATAIN_GPIO, &GPIO_InitStruct);
}

void ParaIO_Init_DMA_In(void)
{
  __HAL_RCC_DMA2_CLK_ENABLE();

  ParaIO_DMA_In.Instance = DMA2_Stream1;
  ParaIO_DMA_In.Init.Channel = DMA_CHANNEL_7;
  ParaIO_DMA_In.Init.Direction = DMA_PERIPH_TO_MEMORY;
  ParaIO_DMA_In.Init.PeriphInc = DMA_PINC_DISABLE;
  ParaIO_DMA_In.Init.MemInc = DMA_MINC_ENABLE;
  ParaIO_DMA_In.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  ParaIO_DMA_In.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  ParaIO_DMA_In.Init.Mode = DMA_NORMAL;
  ParaIO_DMA_In.Init.Priority = DMA_PRIORITY_VERY_HIGH;
  ParaIO_DMA_In.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  HAL_DMA_DeInit(&ParaIO_DMA_In);
  HAL_DMA_Init(&ParaIO_DMA_In);

  __HAL_LINKDMA(ParaIO_TIM_In,hdma[TIM_DMA_ID_UPDATE],ParaIO_DMA_In);
  
  // DMA Interrupt should be disabled in CubeMX
}

void ParaIO_Start_In(void *destAddr,uint32_t len)
{
  HAL_DMA_Start(&ParaIO_DMA_In,(uint32_t)&(GPIOD->IDR),(uint32_t)destAddr,len);
  __HAL_TIM_ENABLE_DMA(ParaIO_TIM_In, TIM_DMA_UPDATE);
  __HAL_TIM_ENABLE(ParaIO_TIM_In);
}