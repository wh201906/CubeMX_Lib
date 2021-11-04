#include "mod.h"
#include "tim.h"

uint8_t Mod_RxBuf[16];
uint8_t Mod_RxId;

WS2812_Dev *WS2812_currDev;
uint32_t WS2812_counter;
uint32_t WS2812_buf[8];
uint32_t WS2812_currTIMChannel;

TIM_HandleTypeDef SigPara_myhtim1; // used as counter in Freq_LF and in Freq_HF

void WS2812_DMAInit(WS2812_Dev *dev, DMA_Stream_TypeDef *DMAStream, uint32_t DMAChannel, IRQn_Type DMAIRQ)
{
  if ((uint32_t)DMAStream > DMA2_BASE)
    __HAL_RCC_DMA2_CLK_ENABLE();
  else if ((uint32_t)DMAStream > DMA1_BASE)
    __HAL_RCC_DMA1_CLK_ENABLE();

  dev->DMAHandle.Instance = DMAStream;
#if defined(STM32H750xx)
  dev->DMAHandle.Init.Request = DMAChannel;
#else
  dev->DMAHandle.Init.Channel = DMAChannel;
#endif
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
  ((DMA_Stream_TypeDef *)dev->DMAHandle.Instance)->CR |= DMA_IT_TC | DMA_IT_HT;
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
    WS2812_buf[i] = WS2812_currDev->comp0;
  __HAL_TIM_SET_COMPARE(WS2812_currDev->htim, TIMChannel, WS2812_currDev->comp0);
  WS2812_currDev->htim->Instance->EGR = TIM_EGR_UG; // force update
  __HAL_TIM_SET_COUNTER(WS2812_currDev->htim, 0);
  HAL_TIM_PWM_Start(WS2812_currDev->htim, TIMChannel);
  HAL_DMA_Init(&WS2812_currDev->DMAHandle); // necessary there
  __HAL_TIM_ENABLE_DMA(WS2812_currDev->htim, TIM_DMA_UPDATE);
  HAL_DMA_Start(&WS2812_currDev->DMAHandle, (uint32_t)WS2812_buf, (uint32_t) & (WS2812_currDev->htim->Instance->CCR1) + TIMChannel, 8);
  while (((DMA_Stream_TypeDef *)WS2812_currDev->DMAHandle.Instance)->CR & DMA_SxCR_EN)
    ;
}

inline void WS2812_UpdateBuf(void)
{
  uint8_t i;
  if (WS2812_counter >= (WS2812_currDev->len+1) * 8) // boundary condition, modify this if necessary
  {
    __HAL_DMA_DISABLE(&WS2812_currDev->DMAHandle);
    // wait for the last bit
    while(__HAL_TIM_GET_COUNTER(WS2812_currDev->htim) <= __HAL_TIM_GET_COMPARE(WS2812_currDev->htim, WS2812_currTIMChannel)) // boundary condition, modify this if necessary
      ;
    __HAL_TIM_SET_COMPARE(WS2812_currDev->htim, WS2812_currTIMChannel, WS2812_currDev->comp0);
  }
  i = 4;
  while (i--)
  {
    WS2812_buf[WS2812_counter % 8] = WS2812_currDev->data[WS2812_counter / 8] << (WS2812_counter % 8) & 0x80 ? WS2812_currDev->comp1 : WS2812_currDev->comp0;
    WS2812_counter++;
  }
  __HAL_DMA_CLEAR_FLAG(&WS2812_currDev->DMAHandle, __HAL_DMA_GET_HT_FLAG_INDEX(&WS2812_currDev->DMAHandle) | __HAL_DMA_GET_TC_FLAG_INDEX(&WS2812_currDev->DMAHandle));
}

static void SigPara_PWM_TIM_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  __HAL_RCC_TIM2_CLK_ENABLE();
  SigPara_myhtim1.Instance = TIM2;
  SigPara_myhtim1.Init.Prescaler = 200;
  SigPara_myhtim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  SigPara_myhtim1.Init.Period = 0xFFFFFFFF;
  SigPara_myhtim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  SigPara_myhtim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_Base_Init(&SigPara_myhtim1);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&SigPara_myhtim1, &sClockSourceConfig);

  HAL_TIM_IC_Init(&SigPara_myhtim1);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  HAL_TIM_SlaveConfigSynchro(&SigPara_myhtim1, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&SigPara_myhtim1, &sMasterConfig);

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  HAL_TIM_IC_ConfigChannel(&SigPara_myhtim1, &sConfigIC, TIM_CHANNEL_1);

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  HAL_TIM_IC_ConfigChannel(&SigPara_myhtim1, &sConfigIC, TIM_CHANNEL_2);
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

static void SigPara_PWM_GPIO_Init(void)
{
  SigPara_Freq_LF_GPIO_Init();
}

void SigPara_PWM_Init(void)
{
  SigPara_PWM_TIM_Init();
  SigPara_PWM_GPIO_Init();
  NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  __HAL_TIM_ENABLE_IT(&SigPara_myhtim1, TIM_IT_CC2);
}

void SigPara_PWM()
{
  Mod_RxId = 0;
  HAL_TIM_IC_Start(&SigPara_myhtim1, TIM_CHANNEL_1);
  HAL_TIM_IC_Start(&SigPara_myhtim1, TIM_CHANNEL_2);
}

void TIM2_IRQHandler(void)
{
  uint32_t i;
  Mod_RxBuf[Mod_RxId++] = __HAL_TIM_GET_COMPARE(&SigPara_myhtim1, TIM_CHANNEL_2);
  Mod_RxId %= 16;
  
  if(Mod_RxId == 0)
  {
    for(i = 0; i < 16; i++)
      printf("%d ", Mod_RxBuf[i]);
    printf("\n");
  }
}