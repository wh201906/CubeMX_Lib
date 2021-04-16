#include "sigpara.h"

DMA_HandleTypeDef myhdma;  // used to copy CCR1 as fast as possible in Freq_LF
TIM_HandleTypeDef myhtim1; // used as conuter in Freq_LF and in Freq_HF
TIM_HandleTypeDef myhtim2; // used as timer in Freq_HF
uint32_t ovrTimes;

float32_t SigPara_RMS(const float32_t *data, uint32_t len)
{
  float32_t res;
  arm_rms_f32(data, len, &res);
  return res;
}

static void SigPara_Freq_LF_TIM_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  __HAL_RCC_TIM4_CLK_ENABLE();
  myhtim1.Instance = TIM4;
  myhtim1.Init.Prescaler = 0;
  myhtim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  myhtim1.Init.Period = 65535;
  myhtim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  myhtim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_IC_Init(&myhtim1);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&myhtim1, &sMasterConfig);

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  // Change this for different freq range
  // If the freq is high, use lower filter value.
  // If the freq is low, use higher filter value.
  sConfigIC.ICFilter = 0x5;
  HAL_TIM_IC_ConfigChannel(&myhtim1, &sConfigIC, TIM_CHANNEL_1);
}

static void SigPara_Freq_LF_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void SigPara_Freq_LF_DMA_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();
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
  __HAL_LINKDMA(&myhtim1, hdma[TIM_DMA_ID_CC1], myhdma);
}

void SigPara_Freq_LF_Init(void)
{
  SigPara_Freq_LF_TIM_Init();
  HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
  SigPara_Freq_LF_GPIO_Init();
  SigPara_Freq_LF_DMA_Init();
}

double SigPara_Freq_LF(void)
{
  uint16_t buf[2];
  HAL_DMA_Init(&myhdma);              // to reset the DMA, otherwise the DMA will work only once.
  __HAL_TIM_SET_COUNTER(&myhtim1, 0); // reset counter to 0 to reduce interrupt

  HAL_DMA_Start(&myhdma, (uint32_t)&myhtim1.Instance->CCR1, (uint32_t)buf, 2);

  ovrTimes = 0;
  __HAL_TIM_CLEAR_IT(&myhtim1, TIM_IT_UPDATE);
  __HAL_TIM_ENABLE_IT(&myhtim1, TIM_IT_UPDATE);

  __HAL_TIM_ENABLE(&myhtim1);
  __HAL_TIM_ENABLE_DMA(&myhtim1, TIM_DMA_CC1);
  TIM_CCxChannelCmd(myhtim1.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE); // the first DMA request will be sent after DMA and TIM is ready
  while (!__HAL_DMA_GET_FLAG(&myhdma, DMA_FLAG_TCIF0_4))
    ;
  __HAL_TIM_DISABLE_IT(&myhtim1, TIM_IT_UPDATE);
  __HAL_TIM_DISABLE(&myhtim1);
  __HAL_TIM_DISABLE_DMA(&myhtim1, TIM_DMA_CC1);
  TIM_CCxChannelCmd(myhtim1.Instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);

  return (84000.0 / (buf[1] + ovrTimes * 65536 - buf[0]));
}

void TIM4_IRQHandler(void)
{
  __HAL_TIM_CLEAR_IT(&myhtim1, TIM_IT_UPDATE);
  if ((DMA1->LISR & DMA_FLAG_HTIF0_4) && !(DMA1->LISR & DMA_FLAG_TCIF0_4)) // capturing the second edge
    ovrTimes++;
}