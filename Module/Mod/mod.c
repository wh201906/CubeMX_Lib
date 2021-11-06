#include "mod.h"
#include "tim.h"

uint8_t Mod_RxBuf[32];
uint8_t Mod_RxBufBegin, Mod_RxBufEnd;
uint32_t Mod_RxThre = 250;
uint8_t Mod_RxId;

Mod_Tx_Dev *WS2812_currDev;
uint32_t WS2812_counter;
uint32_t WS2812_buf[32];
uint32_t WS2812_currTIMChannel;

TIM_HandleTypeDef SigPara_myhtim1; // used as counter in Freq_LF and in Freq_HF

void Mod_Tx_DMAInit(Mod_Tx_Dev *dev, DMA_Stream_TypeDef *DMAStream, uint32_t DMAChannel, IRQn_Type DMAIRQ)
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

void Mod_Tx_Init(Mod_Tx_Dev *dev, DMA_Stream_TypeDef *DMAStream, uint32_t DMAChannel, IRQn_Type DMAIRQ, TIM_HandleTypeDef *htim)
{
  uint32_t tmp;
  dev->htim = htim;

  // for PWM mode 1
  tmp = __HAL_TIM_GET_AUTORELOAD(htim);
  dev->comp0 = tmp * 0.32;
  dev->comp1 = tmp - dev->comp0;
  // another way
  // dev->comp0 = 0;
  // dev->comp1 = tmp / 2;
  Mod_Tx_DMAInit(dev, DMAStream, DMAChannel, DMAIRQ);
}

void Mod_Tx_Start(Mod_Tx_Dev *dev, uint32_t TIMChannel)
{
  uint8_t i;
  WS2812_currDev = dev;
  WS2812_currTIMChannel = TIMChannel;
  WS2812_counter = 0;
  for (i = 0; i < 32; i++)
    WS2812_buf[i] = WS2812_currDev->comp1;
  __HAL_TIM_SET_COMPARE(WS2812_currDev->htim, TIMChannel, WS2812_currDev->comp0);
  WS2812_currDev->htim->Instance->EGR = TIM_EGR_UG; // force update
  __HAL_TIM_SET_COUNTER(WS2812_currDev->htim, 0);
  HAL_TIM_PWM_Start(WS2812_currDev->htim, TIMChannel);
  HAL_DMA_Init(&WS2812_currDev->DMAHandle); // necessary there
  __HAL_TIM_ENABLE_DMA(WS2812_currDev->htim, TIM_DMA_UPDATE);
  HAL_DMA_Start(&WS2812_currDev->DMAHandle, (uint32_t)WS2812_buf, (uint32_t) & (WS2812_currDev->htim->Instance->CCR1) + TIMChannel, 32);
}

void Mod_Tx_SetValue(Mod_Tx_Dev *dev, uint16_t data)
{
  dev->data = data;
}

inline uint8_t Mod_Tx_IsFinished(void)
{
  return !(((DMA_Stream_TypeDef *)WS2812_currDev->DMAHandle.Instance)->CR & DMA_SxCR_EN);
}

inline void Mod_Tx_UpdateBuf(void)
{
  uint8_t i;
  if(__HAL_DMA_GET_FLAG(&WS2812_currDev->DMAHandle, DMA_FLAG_TCIF1_5))
  {
    WS2812_buf[16] = WS2812_currDev->comp0;
    for(i = 17; i < 31; i++)
    {
      WS2812_buf[i] = WS2812_currDev->comp1;
    }
    WS2812_buf[31] = WS2812_currDev->comp1;
    __HAL_DMA_CLEAR_FLAG(&WS2812_currDev->DMAHandle, DMA_FLAG_TCIF1_5);
  }
  else if(__HAL_DMA_GET_FLAG(&WS2812_currDev->DMAHandle, DMA_FLAG_HTIF1_5))
  {
    WS2812_buf[0] = WS2812_currDev->comp0;
    for(i = 1; i < 15; i++)
    {
      WS2812_buf[i] = (WS2812_currDev->data << (i - 1) & 0x2000) ? WS2812_currDev->comp1 : WS2812_currDev->comp0;
    }
    WS2812_buf[15] = WS2812_currDev->comp1;
    __HAL_DMA_CLEAR_FLAG(&WS2812_currDev->DMAHandle, DMA_FLAG_HTIF1_5);
  }
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
  Mod_RxBufBegin = 0;
  Mod_RxBufEnd = 0;
  HAL_TIM_IC_Start(&SigPara_myhtim1, TIM_CHANNEL_1);
  HAL_TIM_IC_Start(&SigPara_myhtim1, TIM_CHANNEL_2);
}

/*
void TIM2_IRQHandler(void)
{
  uint32_t i;
  uint32_t data; 
  Mod_RxBuf[Mod_RxBufEnd] = __HAL_TIM_GET_COMPARE(&SigPara_myhtim1, TIM_CHANNEL_2) < Mod_RxThre ? 0 : 1;
  Mod_RxBufEnd++;
  Mod_RxBufEnd %= 32;
  if(Mod_RxBufBegin == Mod_RxBufEnd) // full
  {
    data = 0;
    for(i = Mod_RxBufBegin; (i + 1) % 32 != Mod_RxBufEnd; i++)
    {
      data <<= 1;
      data |= Mod_RxBuf[i % 32];
    }
    data <<= 1;
    data |= Mod_RxBuf[i % 32];
    if(!(data & 0x80000000u) && (data & 0x1FFFFu) == 0x17FFFu) // pattern detected
    {
      data >>= 16 + 1;
      data &= 0x3FFF;
      printf("%u\n", data);
    }
    Mod_RxBufBegin++;
    Mod_RxBufBegin %= 32;
  }
}
*/

inline void Mod_Rx_Read(uint8_t bit)
{
  uint32_t i;
  uint32_t data; 
  Mod_RxBuf[Mod_RxBufEnd] = bit;
  Mod_RxBufEnd++;
  Mod_RxBufEnd %= 32;
  if(Mod_RxBufBegin == Mod_RxBufEnd) // full
  {
    data = 0;
    for(i = Mod_RxBufBegin; (i + 1) % 32 != Mod_RxBufEnd; i++)
    {
      data <<= 1;
      data |= Mod_RxBuf[i % 32];
    }
    data <<= 1;
    data |= Mod_RxBuf[i % 32];
    if(!(data & 0x80000000u) && (data & 0x1FFFFu) == 0x17FFFu) // pattern detected
    {
      data >>= 16 + 1;
      data &= 0x3FFF;
      printf("%u\n", data);
    }
    Mod_RxBufBegin++;
    Mod_RxBufBegin %= 32;
  }
}