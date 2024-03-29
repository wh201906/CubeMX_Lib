#include "paraio.h"

// on STM32F4, GPIO is on AHB1, which is only accessible by DMA2
// the Timer should be configured by CubeMX

DMA_HandleTypeDef ParaIO_DMA_In;
DMA_HandleTypeDef ParaIO_DMA_In2;
DMA_HandleTypeDef ParaIO_DMA_Out;
TIM_HandleTypeDef *ParaIO_TIM_In;
TIM_HandleTypeDef *ParaIO_TIM_In2;
TIM_HandleTypeDef *ParaIO_TIM_Out;

void ParaIO_Init_GPIO_In(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  PARAIO_DATAIN_CLKEN();

  GPIO_InitStruct.Pin = GPIO_PIN_All;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(PARAIO_DATAIN_GPIO, &GPIO_InitStruct);
}

void ParaIO_Init_GPIO_In2(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  PARAIO_DATAIN2_CLKEN();

  GPIO_InitStruct.Pin = GPIO_PIN_All;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(PARAIO_DATAIN2_GPIO, &GPIO_InitStruct);
}

// 32bit is not implemented there since a single GPIO only have 16 pins
void ParaIO_Init_DMA_In(uint8_t memUnitSize, uint8_t isCircular) // 8,16
{
  __HAL_RCC_DMA2_CLK_ENABLE();

  // On F407, 
  // TIM8_UP is linked to DMA2_Stream1_Channel7
  // TIM1_UP is linked to DMA2_Stream5_Channel6
  // TIM8_CH1 is linked to DMA2_Stream2_Channel7
  // TIM1_CH2 is linked to DMA2_Stream2_Channel6
  // TIM8_CH2 is linked to DMA2_Stream3_Channel7
  // TIM1_CH1 is linked to DMA2_Stream3_Channel6
  
  // Remember to change ParaIO_IsTranferCompleted_In()
  ParaIO_DMA_In.Instance = DMA2_Stream2;
  ParaIO_DMA_In.Init.Channel = DMA_CHANNEL_7;
  ParaIO_DMA_In.Init.Direction = DMA_PERIPH_TO_MEMORY;
  ParaIO_DMA_In.Init.PeriphInc = DMA_PINC_DISABLE;
  ParaIO_DMA_In.Init.MemInc = DMA_MINC_ENABLE;
  if (memUnitSize == 8)
  {
    ParaIO_DMA_In.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    ParaIO_DMA_In.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  }
  else if (memUnitSize == 16)
  {
    ParaIO_DMA_In.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    ParaIO_DMA_In.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  }
  ParaIO_DMA_In.Init.Mode = isCircular ? DMA_CIRCULAR : DMA_NORMAL;
  ParaIO_DMA_In.Init.Priority = DMA_PRIORITY_VERY_HIGH;
  ParaIO_DMA_In.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  HAL_DMA_DeInit(&ParaIO_DMA_In);
  HAL_DMA_Init(&ParaIO_DMA_In);

  __HAL_LINKDMA(ParaIO_TIM_In, hdma[TIM_DMA_ID_CC1], ParaIO_DMA_In);

  // DMA Interrupt should be disabled in CubeMX
}

void ParaIO_Init_DMA_In2(uint8_t memUnitSize, uint8_t isCircular) // 8,16
{
  __HAL_RCC_DMA2_CLK_ENABLE();
  
  // Remember to change ParaIO_IsTranferCompleted_In2()
  ParaIO_DMA_In2.Instance = DMA2_Stream3;
  ParaIO_DMA_In2.Init.Channel = DMA_CHANNEL_7;
  ParaIO_DMA_In2.Init.Direction = DMA_PERIPH_TO_MEMORY;
  ParaIO_DMA_In2.Init.PeriphInc = DMA_PINC_DISABLE;
  ParaIO_DMA_In2.Init.MemInc = DMA_MINC_ENABLE;
  if (memUnitSize == 8)
  {
    ParaIO_DMA_In2.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    ParaIO_DMA_In2.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  }
  else if (memUnitSize == 16)
  {
    ParaIO_DMA_In2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    ParaIO_DMA_In2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  }
  ParaIO_DMA_In2.Init.Mode = isCircular ? DMA_CIRCULAR : DMA_NORMAL;
  ParaIO_DMA_In2.Init.Priority = DMA_PRIORITY_HIGH; // lower priority than ParaIO_DMA_In
  ParaIO_DMA_In2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  HAL_DMA_DeInit(&ParaIO_DMA_In2);
  HAL_DMA_Init(&ParaIO_DMA_In2);

  __HAL_LINKDMA(ParaIO_TIM_In2, hdma[TIM_DMA_ID_CC2], ParaIO_DMA_In2);

  // DMA Interrupt should be disabled in CubeMX
}

void ParaIO_Init_In(TIM_HandleTypeDef *htim, uint8_t memUnitSize, uint8_t isCircular)
{
  ParaIO_TIM_In = htim;
  ParaIO_Init_GPIO_In();
  ParaIO_Init_DMA_In(memUnitSize, isCircular);
}

void ParaIO_Init_In2(TIM_HandleTypeDef *htim, uint8_t memUnitSize, uint8_t isCircular)
{
  ParaIO_TIM_In2 = htim;
  ParaIO_Init_GPIO_In2();
  ParaIO_Init_DMA_In2(memUnitSize, isCircular);
}

void ParaIO_Start_In(void *destAddr, uint32_t len)
{
  HAL_DMA_Init(&ParaIO_DMA_In); // necessary there
  HAL_DMA_Start(&ParaIO_DMA_In, (uint32_t) & (PARAIO_DATAIN_GPIO->IDR), (uint32_t)destAddr, len);
  __HAL_TIM_ENABLE_DMA(ParaIO_TIM_In, TIM_DMA_CC1);
  __HAL_TIM_ENABLE(ParaIO_TIM_In);
}

void ParaIO_Start_In2(void *destAddr, uint32_t len)
{
  HAL_DMA_Init(&ParaIO_DMA_In2); // necessary there
  HAL_DMA_Start(&ParaIO_DMA_In2, (uint32_t) & (PARAIO_DATAIN2_GPIO->IDR), (uint32_t)destAddr, len);
  __HAL_TIM_ENABLE_DMA(ParaIO_TIM_In2, TIM_DMA_CC2);
  __HAL_TIM_ENABLE(ParaIO_TIM_In2);
}

void ParaIO_Start_In_Sync(void *destAddr1, uint32_t len1, void *destAddr2, uint32_t len2)
{
  // the ParaIO_TIM_In and ParaIO_TIM_In2 should be the same
  
  HAL_DMA_Init(&ParaIO_DMA_In); // necessary there
  HAL_DMA_Start(&ParaIO_DMA_In, (uint32_t) & (PARAIO_DATAIN_GPIO->IDR), (uint32_t)destAddr1, len1);
  __HAL_TIM_ENABLE_DMA(ParaIO_TIM_In, TIM_DMA_CC1);
  ParaIO_Start_In2(destAddr2, len2);
}

void ParaIO_Init_GPIO_Out(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  PARAIO_DATAOUT_CLKEN();

  GPIO_InitStruct.Pin = GPIO_PIN_All;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(PARAIO_DATAOUT_GPIO, &GPIO_InitStruct);
}

// 32bit is not implemented there since a single GPIO only have 16 pins
void ParaIO_Init_DMA_Out(uint8_t memUnitSize, uint8_t isCircular) // 8,16
{
  __HAL_RCC_DMA2_CLK_ENABLE();

  // On F407, TIM8_UP is linked to DMA2_Stream1_Channel7
  // TIM1_UP is linked to DMA2_Stream5_Channel6
  
  // Remember to change ParaIO_IsTranferCompleted_Out()
  ParaIO_DMA_Out.Instance = DMA2_Stream1;
  ParaIO_DMA_Out.Init.Channel = DMA_CHANNEL_7;
  ParaIO_DMA_Out.Init.Direction = DMA_MEMORY_TO_PERIPH;
  ParaIO_DMA_Out.Init.PeriphInc = DMA_PINC_DISABLE;
  ParaIO_DMA_Out.Init.MemInc = DMA_MINC_ENABLE;
  if (memUnitSize == 8)
  {
    ParaIO_DMA_Out.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    ParaIO_DMA_Out.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  }
  else if (memUnitSize == 16)
  {
    ParaIO_DMA_Out.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    ParaIO_DMA_Out.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  }
  ParaIO_DMA_Out.Init.Mode = isCircular ? DMA_CIRCULAR : DMA_NORMAL;
  ParaIO_DMA_Out.Init.Priority = DMA_PRIORITY_VERY_HIGH;
  ParaIO_DMA_Out.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  HAL_DMA_DeInit(&ParaIO_DMA_Out);
  HAL_DMA_Init(&ParaIO_DMA_Out);

  __HAL_LINKDMA(ParaIO_TIM_Out, hdma[TIM_DMA_ID_UPDATE], ParaIO_DMA_Out);

  // DMA Interrupt should be disabled in CubeMX
}

void ParaIO_Init_Out(TIM_HandleTypeDef *htim, uint8_t memUnitSize, uint8_t isCircular)
{
  ParaIO_TIM_Out = htim;
  ParaIO_Init_GPIO_Out();
  ParaIO_Init_DMA_Out(memUnitSize, isCircular);
}

void ParaIO_Start_Out(void *srcAddr, uint32_t len)
{
  HAL_DMA_Init(&ParaIO_DMA_Out); // necessary there
  HAL_DMA_Start(&ParaIO_DMA_Out, (uint32_t)srcAddr, (uint32_t) & (PARAIO_DATAOUT_GPIO->ODR), len);
  __HAL_TIM_ENABLE_DMA(ParaIO_TIM_Out, TIM_DMA_UPDATE);
  __HAL_TIM_ENABLE(ParaIO_TIM_Out);
}

uint8_t ParaIO_IsTranferCompleted_Out(void)
{
  // On F407, TIM8_UP is linked to DMA2_Stream1_Channel7
  // TIM1_UP is linked to DMA2_Stream5_Channel6
  return __HAL_DMA_GET_FLAG(&ParaIO_DMA_Out, DMA_FLAG_TCIF1_5) == DMA_FLAG_TCIF1_5;
}

uint8_t ParaIO_IsTranferCompleted_In(void)
{
  // On F407, TIM8_UP is linked to DMA2_Stream1_Channel7
  // TIM1_UP is linked to DMA2_Stream5_Channel6
  return __HAL_DMA_GET_FLAG(&ParaIO_DMA_In, DMA_FLAG_TCIF2_6) == DMA_FLAG_TCIF2_6;
}

uint8_t ParaIO_IsTranferCompleted_In2(void)
{
  // On F407, TIM8_UP is linked to DMA2_Stream1_Channel7
  // TIM1_UP is linked to DMA2_Stream5_Channel6
  return __HAL_DMA_GET_FLAG(&ParaIO_DMA_In2, DMA_FLAG_TCIF3_7) == DMA_FLAG_TCIF3_7;
}