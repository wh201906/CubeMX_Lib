#include "paraio.h"

// on STM32F4, GPIO is on AHB1, which is only accessible by DMA2

DMA_HandleTypeDef ParaIO_DMA_In;
TIM_HandleTypeDef ParaIO_TIM_In;

void ParaIO_Init_GPIO_In(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  PARAIO_DATAIN_CLKEN(void);

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
  ParaIO_DMA_In.Init.Mode = DMA_CIRCULAR;
  ParaIO_DMA_In.Init.Priority = DMA_PRIORITY_VERY_HIGH;
  ParaIO_DMA_In.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  HAL_DMA_DeInit(&ParaIO_DMA_In);
  HAL_DMA_Init(&ParaIO_DMA_In);

  __HAL_LINKDMA(&ParaIO_TIM_In,hdma[TIM_DMA_ID_UPDATE],ParaIO_DMA_In);

  ParaIO_TIM_In.hdma[TIM_DMA_ID_UPDATE]->XferCpltCallback = TIM_DMADelayPulseCplt;
  ParaIO_TIM_In.hdma[TIM_DMA_ID_UPDATE]->XferHalfCpltCallback = TIM_DMADelayPulseHalfCplt;
  ParaIO_TIM_In.hdma[TIM_DMA_ID_UPDATE]->XferErrorCallback = TIM_DMAError;

}

void ParaIO_Init_TIM_In(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  // The DMA2 can only be triggered by TIM1/TIM8, which have BreakDeadTimeConfig
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0}; 

  ParaIO_TIM_In.Instance = TIM8;
  ParaIO_TIM_In.Init.Prescaler = 9;
  ParaIO_TIM_In.Init.CounterMode = TIM_COUNTERMODE_UP;
  ParaIO_TIM_In.Init.Period = 167;
  ParaIO_TIM_In.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  ParaIO_TIM_In.Init.RepetitionCounter = 0;
  ParaIO_TIM_In.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
}