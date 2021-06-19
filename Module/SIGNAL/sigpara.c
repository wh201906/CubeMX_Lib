#include "sigpara.h"

DMA_HandleTypeDef SigPara_myhdma;  // used to copy CCR1 as fast as possible in Freq_LF
TIM_HandleTypeDef SigPara_myhtim1; // used as counter in Freq_LF and in Freq_HF
TIM_HandleTypeDef SigPara_myhtim2; // used as timer in Freq_HF
uint32_t SigPara_ovrTimes;
uint32_t SigPara_thre_Tout, SigPara_thre_Ticks, SigPara_thre_P;
double SigPara_thre_F;

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
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};

  __HAL_RCC_TIM2_CLK_ENABLE();
  SigPara_myhtim1.Instance = TIM2;
  SigPara_myhtim1.Init.Prescaler = 0;
  SigPara_myhtim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  SigPara_myhtim1.Init.Period = 0xFFFFFFFF;
  SigPara_myhtim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  SigPara_myhtim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_IC_Init(&SigPara_myhtim1);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&SigPara_myhtim1, &sClockSourceConfig);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE;
  HAL_TIM_SlaveConfigSynchro(&SigPara_myhtim1, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&SigPara_myhtim1, &sMasterConfig);

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  // Change this for different freq range
  // If the freq is high, use lower filter value.
  // If the freq is low, use higher filter value.
  // This can reduce the precision
  sConfigIC.ICFilter = 0x5;
  HAL_TIM_IC_ConfigChannel(&SigPara_myhtim1, &sConfigIC, TIM_CHANNEL_1);
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
  SigPara_myhdma.Instance = DMA1_Stream5;
  SigPara_myhdma.Init.Channel = DMA_CHANNEL_3;
  SigPara_myhdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
  SigPara_myhdma.Init.PeriphInc = DMA_PINC_DISABLE;
  SigPara_myhdma.Init.MemInc = DMA_MINC_ENABLE;
  SigPara_myhdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  SigPara_myhdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  SigPara_myhdma.Init.Mode = DMA_NORMAL;
  SigPara_myhdma.Init.Priority = DMA_PRIORITY_LOW;
  SigPara_myhdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  HAL_DMA_Init(&SigPara_myhdma);
  __HAL_LINKDMA(&SigPara_myhtim1, hdma[TIM_DMA_ID_CC1], SigPara_myhdma);
}

void SigPara_Freq_LF_Init(void)
{
  SigPara_Freq_LF_TIM_Init();
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  SigPara_Freq_LF_GPIO_Init();
  SigPara_Freq_LF_DMA_Init();
}

double SigPara_Freq_LF(uint32_t timeout) // timeout in ms
{
  uint32_t buf[2];
  HAL_DMA_Init(&SigPara_myhdma);              // to reset the DMA, otherwise the DMA will work only once.
  __HAL_TIM_SET_COUNTER(&SigPara_myhtim1, 0); // reset counter to 0 to reduce interrupt

  HAL_DMA_Start(&SigPara_myhdma, (uint32_t)&SigPara_myhtim1.Instance->CCR1, (uint32_t)buf, 2);

  SigPara_ovrTimes = 0;
  __HAL_TIM_CLEAR_IT(&SigPara_myhtim1, TIM_IT_UPDATE);
  __HAL_TIM_ENABLE_IT(&SigPara_myhtim1, TIM_IT_UPDATE);

  __HAL_TIM_ENABLE(&SigPara_myhtim1);
  __HAL_TIM_ENABLE_DMA(&SigPara_myhtim1, TIM_DMA_CC1);
  TIM_CCxChannelCmd(SigPara_myhtim1.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE); // the first DMA request will be sent after DMA and TIM is ready
  while (!__HAL_DMA_GET_FLAG(&SigPara_myhdma, DMA_FLAG_TCIF1_5) && timeout--)
    Delay_ms(1);
  __HAL_TIM_DISABLE_IT(&SigPara_myhtim1, TIM_IT_UPDATE);
  __HAL_TIM_DISABLE(&SigPara_myhtim1);
  __HAL_TIM_DISABLE_DMA(&SigPara_myhtim1, TIM_DMA_CC1);
  TIM_CCxChannelCmd(SigPara_myhtim1.Instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);

  return (SIGPARA_HTIM1_CLK / (buf[1] + SigPara_ovrTimes * (__HAL_TIM_GET_AUTORELOAD(&SigPara_myhtim1) + 1) - buf[0]) / (SigPara_myhtim1.Instance->PSC + 1));
}

static void SigPara_Freq_HF_TimerTIM_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  __HAL_RCC_TIM3_CLK_ENABLE();
  SigPara_myhtim2.Instance = TIM3;
  SigPara_myhtim2.Init.Prescaler = 0;
  SigPara_myhtim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  SigPara_myhtim2.Init.Period = 65535;
  SigPara_myhtim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  SigPara_myhtim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_Base_Init(&SigPara_myhtim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&SigPara_myhtim2, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&SigPara_myhtim2);
  HAL_TIM_OnePulse_Init(&SigPara_myhtim2, TIM_OPMODE_SINGLE);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1REF;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  HAL_TIMEx_MasterConfigSynchronization(&SigPara_myhtim2, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&SigPara_myhtim2, &sConfigOC, TIM_CHANNEL_1);
}

static void SigPara_Freq_HF_CounterTIM_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  __HAL_RCC_TIM2_CLK_ENABLE();
  SigPara_myhtim1.Instance = TIM2;
  SigPara_myhtim1.Init.Prescaler = 0;
  SigPara_myhtim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  SigPara_myhtim1.Init.Period = 0xFFFFFFFF;
  SigPara_myhtim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  SigPara_myhtim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_Base_Init(&SigPara_myhtim1);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  sClockSourceConfig.ClockFilter = 0;
  HAL_TIM_ConfigClockSource(&SigPara_myhtim1, &sClockSourceConfig);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_GATED;
  sSlaveConfig.InputTrigger = TIM_TS_ITR2;
  HAL_TIM_SlaveConfigSynchro(&SigPara_myhtim1, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&SigPara_myhtim1, &sMasterConfig);
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

double SigPara_Freq_HF(uint32_t Ticks, uint16_t P) // The width of Ticks depends on the width of arr of SigPara_myhtim2, 16 is the default width
{
  uint32_t edgeNum;
  __HAL_TIM_SET_COUNTER(&SigPara_myhtim1, 0);
  __HAL_TIM_SET_COUNTER(&SigPara_myhtim2, 0);
  __HAL_TIM_SET_AUTORELOAD(&SigPara_myhtim2, Ticks - 1);
  __HAL_TIM_SET_PRESCALER(&SigPara_myhtim2, P - 1);
  // Generate an update event to reload the Prescaler
  // and the repetition counter (only for advanced timer) value immediately
  // This instruction is extracted from TIM_Base_SetConfig() in stm32f4xx_hal_tim.c
  SigPara_myhtim2.Instance->EGR = TIM_EGR_UG;

  SigPara_ovrTimes = 0;
  __HAL_TIM_CLEAR_IT(&SigPara_myhtim1, TIM_IT_UPDATE);
  __HAL_TIM_ENABLE_IT(&SigPara_myhtim1, TIM_IT_UPDATE);

  __HAL_TIM_ENABLE(&SigPara_myhtim1);
  __HAL_TIM_ENABLE(&SigPara_myhtim2);                 // start timer after the counter is enabled for better precision
  while (SigPara_myhtim2.Instance->CR1 & TIM_CR1_CEN) // still counting
    ;
  edgeNum = __HAL_TIM_GET_COUNTER(&SigPara_myhtim1) + SigPara_ovrTimes * (__HAL_TIM_GET_AUTORELOAD(&SigPara_myhtim1) + 1);
  return (SIGPARA_HTIM2_CLK / Ticks * edgeNum / (SigPara_myhtim2.Instance->PSC + 1));
}

uint64_t SigPara_Freq_Auto_SetMinPrecision(double permillage)
{
  // Tested with LF_ICFilter = 0x5
  uint64_t extraTicks;
  SigPara_thre_F = SIGPARA_HTIM1_CLK / 1000 * permillage / 2;
  //SigPara_thre_F = 10000;
  extraTicks = SIGPARA_HTIM2_CLK / SigPara_thre_F / permillage * 1000;
  SigPara_thre_P = SIGPARA_HTIM2_CLK / 100000; // P*100kHz=HTIM2_CLK, P contains factors in HTIM2_CLK
  SigPara_thre_Ticks = extraTicks / SigPara_thre_P + 1;
  if (SigPara_thre_Ticks > 65535)
  {
    SigPara_thre_P = SIGPARA_HTIM2_CLK / 10000; // P*10kHz=HTIM2_CLK, P contains factors in HTIM2_CLK
    SigPara_thre_Ticks = extraTicks / SigPara_thre_P + 1;
  }
  return extraTicks;
}

void SigPara_Freq_Auto_SetMaxTimeout(uint32_t ms)
{
  SigPara_thre_Tout = ms;
}

double SigPara_Freq_Auto(void) // Frequency measurement with auto range
{
  // Prec(%)
  // ^
  // |
  // |     .             .
  // |      .           .
  // |       .         .
  // |       .         .
  // |        .       .
  // |         .     .
  // |          .   .
  // |           ...
  // |            ^
  // |         Lowest
  // -------------|-----------------> Freq
  //    Freq_LF()       Freq_HF()
  // The F, Ticks, P is determined by precision_min
  // The Tout is determined by timeout_max
  // Freq >= 1.5MHz : Tmeas = (Ticks * P + 1) / HTIM2_CLK + 1/1500000
  // F <= Freq < 1.5MHz : Tmeas = (Ticks * P + 1) / HTIM2_CLK + 1/1500000 + 1/Freq
  //     Worst case there:
  //     F = Freq : Tmeas = (2 * Ticks * P + 1) / HTIM2_CLK + 1/1500000
  // Freq < F : Tmeas = 1/HTIM2_CLK + 1/1500000 + Tout
  double freq;

  // Stage 1
  // Frequency higher than 1.5M cannot be measured by Freq_LF()
  SigPara_Freq_HF_Init();
  freq = SigPara_Freq_HF((SIGPARA_HTIM2_CLK / 1500000 + 1), 1);
  if (freq >= 1500000)
    return SigPara_Freq_HF(SigPara_thre_Ticks, SigPara_thre_P);

  // Stage 2
  // Measure with Freq_LF()
  // If the frequency is lower than thresholdF, the result is precise enough.
  // If not, measure with Freq_HF()
  SigPara_Freq_LF_Init();
  freq = SigPara_Freq_LF(SigPara_thre_Tout);
  if (freq < SigPara_thre_F)
    return freq;

  // Stage 3
  // The frequency is between thresholdF and 1.5M, measure with Freq_HF()
  SigPara_Freq_HF_Init();
  return SigPara_Freq_HF(SigPara_thre_Ticks, SigPara_thre_P);
}

/*
void TIM4_IRQHandler(void)
{
  __HAL_TIM_CLEAR_IT(&SigPara_myhtim1, TIM_IT_UPDATE);
  if ((DMA1->LISR & DMA_FLAG_HTIF0_4) && !(DMA1->LISR & DMA_FLAG_TCIF0_4)) // capturing the second edge
    SigPara_ovrTimes++;
  else if (SigPara_myhtim2.Instance->CR1 & TIM_CR1_CEN)
    SigPara_ovrTimes++;
}
*/
void TIM2_IRQHandler(void)
{
  __HAL_TIM_CLEAR_IT(&SigPara_myhtim1, TIM_IT_UPDATE);
  if ((DMA1->HISR & DMA_FLAG_HTIF1_5) && !(DMA1->HISR & DMA_FLAG_TCIF1_5)) // capturing the second edge
    SigPara_ovrTimes++;
  else if (SigPara_myhtim2.Instance->CR1 & TIM_CR1_CEN)
    SigPara_ovrTimes++;
}