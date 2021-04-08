#include "sigpara.h"

DMA_HandleTypeDef myhdma;
TIM_HandleTypeDef myhtim;

float32_t SigPara_RMS(const float32_t *data, uint32_t len)
{
  float32_t res;
  arm_rms_f32(data, len, &res);
  return res;
}

void SigPara_Freq_LF_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};
  
  __HAL_RCC_TIM4_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  
  myhtim.Instance = TIM4;
  myhtim.Init.Prescaler = 0;
  myhtim.Init.CounterMode = TIM_COUNTERMODE_UP;
  myhtim.Init.Period = 65535;
  myhtim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  myhtim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_IC_Init(&myhtim);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&myhtim, &sMasterConfig);

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  HAL_TIM_IC_ConfigChannel(&myhtim, &sConfigIC, TIM_CHANNEL_1);

  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  myhdma.Instance = DMA1_Stream0;
  myhdma.Init.Channel = DMA_CHANNEL_2;
  myhdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
  myhdma.Init.PeriphInc = DMA_PINC_DISABLE;
  myhdma.Init.MemInc = DMA_MINC_ENABLE;
  myhdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  myhdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  myhdma.Init.Mode = DMA_NORMAL;
  myhdma.Init.Priority = DMA_PRIORITY_LOW;
  myhdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  HAL_DMA_Init(&myhdma);
  __HAL_LINKDMA(&myhtim, hdma[TIM_DMA_ID_CC1], myhdma);
}

double SigPara_Freq_LF(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length)
{
  HAL_DMA_Init(&myhdma); // to reset the DMA, otherwise the DMA will work only once.
  myhtim.hdma[TIM_DMA_ID_CC1]->XferCpltCallback = TIM_DMACaptureCplt;
  myhtim.hdma[TIM_DMA_ID_CC1]->XferHalfCpltCallback = TIM_DMACaptureHalfCplt;

  myhtim.hdma[TIM_DMA_ID_CC1]->XferErrorCallback = TIM_DMAError;

  HAL_DMA_Start_IT(myhtim.hdma[TIM_DMA_ID_CC1], (uint32_t)&myhtim.Instance->CCR1, (uint32_t)pData, Length);

  __HAL_TIM_ENABLE_DMA(&myhtim, TIM_DMA_CC1);

  TIM_CCxChannelCmd(myhtim.Instance, Channel, TIM_CCx_ENABLE);

  __HAL_TIM_ENABLE(&myhtim);

  return 0;
}