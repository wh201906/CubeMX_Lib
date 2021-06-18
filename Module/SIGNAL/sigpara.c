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

  __HAL_RCC_TIM2_CLK_ENABLE();
  myhtim1.Instance = TIM2;
  myhtim1.Init.Prescaler = 0;
  myhtim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  myhtim1.Init.Period = 0xFFFFFFFF;
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

  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void SigPara_Freq_LF_DMA_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();
  myhdma.Instance = DMA1_Stream5;
  myhdma.Init.Channel = DMA_CHANNEL_3;
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
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
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
  while (!__HAL_DMA_GET_FLAG(&myhdma, DMA_FLAG_TCIF1_5))
    ;
  __HAL_TIM_DISABLE_IT(&myhtim1, TIM_IT_UPDATE);
  __HAL_TIM_DISABLE(&myhtim1);
  __HAL_TIM_DISABLE_DMA(&myhtim1, TIM_DMA_CC1);
  TIM_CCxChannelCmd(myhtim1.Instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);

  return (SIGPARA_HTIM1_CLK / (buf[1] + ovrTimes * (__HAL_TIM_GET_AUTORELOAD(&myhtim1) + 1) - buf[0]) / (myhtim1.Instance->PSC + 1));
  ;
}

static void SigPara_Freq_HF_TimerTIM_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  __HAL_RCC_TIM3_CLK_ENABLE();
  myhtim2.Instance = TIM3;
  myhtim2.Init.Prescaler = 0;
  myhtim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  myhtim2.Init.Period = 65535;
  myhtim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  myhtim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&myhtim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&myhtim2, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&myhtim2);
  HAL_TIM_OnePulse_Init(&myhtim2, TIM_OPMODE_SINGLE);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1REF;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  HAL_TIMEx_MasterConfigSynchronization(&myhtim2, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&myhtim2, &sConfigOC, TIM_CHANNEL_1);
}

static void SigPara_Freq_HF_CounterTIM_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  __HAL_RCC_TIM2_CLK_ENABLE();
  myhtim1.Instance = TIM2;
  myhtim1.Init.Prescaler = 0;
  myhtim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  myhtim1.Init.Period = 0xFFFFFFFF;
  myhtim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  myhtim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&myhtim1);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  sClockSourceConfig.ClockFilter = 0;
  HAL_TIM_ConfigClockSource(&myhtim1, &sClockSourceConfig);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_GATED;
  sSlaveConfig.InputTrigger = TIM_TS_ITR2;
  HAL_TIM_SlaveConfigSynchro(&myhtim1, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&myhtim1, &sMasterConfig);
}

static void SigPara_Freq_HF_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SigPara_Freq_HF_Init(void)
{
  SigPara_Freq_HF_TimerTIM_Init();
  SigPara_Freq_HF_CounterTIM_Init();
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  SigPara_Freq_HF_GPIO_Init();
}

double SigPara_Freq_HF(uint32_t countTimes, uint16_t P) // The width of countTimes depends on the width of arr of myhtim2, 16 is the default width
{
  uint32_t edgeNum;
  __HAL_TIM_SET_COUNTER(&myhtim1, 0);
  __HAL_TIM_SET_COUNTER(&myhtim2, 0);
  __HAL_TIM_SET_AUTORELOAD(&myhtim2, countTimes - 1);
  __HAL_TIM_SET_PRESCALER(&myhtim2,  P - 1);

  ovrTimes = 0;
  __HAL_TIM_CLEAR_IT(&myhtim1, TIM_IT_UPDATE);
  __HAL_TIM_ENABLE_IT(&myhtim1, TIM_IT_UPDATE);

  __HAL_TIM_ENABLE(&myhtim1);
  __HAL_TIM_ENABLE(&myhtim2);                 // start timer after the counter is enabled for better precision
  while (myhtim2.Instance->CR1 & TIM_CR1_CEN) // still counting
    ;
  edgeNum = __HAL_TIM_GET_COUNTER(&myhtim1) + ovrTimes * (__HAL_TIM_GET_AUTORELOAD(&myhtim1) + 1);
  return (SIGPARA_HTIM2_CLK / countTimes * edgeNum / (myhtim2.Instance->PSC + 1));
}
/*
void TIM4_IRQHandler(void)
{
  __HAL_TIM_CLEAR_IT(&myhtim1, TIM_IT_UPDATE);
  if ((DMA1->LISR & DMA_FLAG_HTIF0_4) && !(DMA1->LISR & DMA_FLAG_TCIF0_4)) // capturing the second edge
    ovrTimes++;
  else if (myhtim2.Instance->CR1 & TIM_CR1_CEN)
    ovrTimes++;
}
*/
void TIM2_IRQHandler(void)
{
  __HAL_TIM_CLEAR_IT(&myhtim1, TIM_IT_UPDATE);
  if ((DMA1->HISR & DMA_FLAG_HTIF1_5) && !(DMA1->HISR & DMA_FLAG_TCIF1_5)) // capturing the second edge
    ovrTimes++;
  else if (myhtim2.Instance->CR1 & TIM_CR1_CEN)
    ovrTimes++;
}