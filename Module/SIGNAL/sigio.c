#include "sigio.h"

DMA_HandleTypeDef SigIO_DMA_ADC;
DMA_HandleTypeDef SigIO_DMA_DAC;
TIM_HandleTypeDef *SigIO_TIM;
ADC_HandleTypeDef *SigIO_ADC;
uint8_t nextADCPtr;
uint8_t nextDACPtr;
uint32_t SigIO_PartLen;

// REMEMBER TO CHANGE THE TYPE OF THIS VARIABLE!!!
// REMEMBER TO CHANGE THE TYPE OF THIS VARIABLE!!!
// REMEMBER TO CHANGE THE TYPE OF THIS VARIABLE!!!
SigIO_TargetType *SigIO_BaseBufAddr;

void (*SigIO_ProcFunc)(SigIO_TargetType *addr, uint32_t len);

void SigIO_Init(TIM_HandleTypeDef *htim, ADC_HandleTypeDef *hadc)
{
  SigIO_TIM = htim;
  SigIO_ADC = hadc;
  SigIO_DMA_Init();
  SigIO_ProcFunc = NULL;
}

void SigIO_DMA_Init()
{
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  // DMA for DAC, Triggered by TIM
  SigIO_DMA_DAC.Instance = DMA1_Stream1;
  SigIO_DMA_DAC.Init.Channel = DMA_CHANNEL_3;
  SigIO_DMA_DAC.Init.Direction = DMA_MEMORY_TO_PERIPH;
  SigIO_DMA_DAC.Init.PeriphInc = DMA_PINC_DISABLE;
  SigIO_DMA_DAC.Init.MemInc = DMA_MINC_ENABLE;
  SigIO_DMA_DAC.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  SigIO_DMA_DAC.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  SigIO_DMA_DAC.Init.Mode = DMA_CIRCULAR;
  SigIO_DMA_DAC.Init.Priority = DMA_PRIORITY_VERY_HIGH;
  SigIO_DMA_DAC.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  HAL_DMA_DeInit(&SigIO_DMA_DAC);
  HAL_DMA_Init(&SigIO_DMA_DAC);

  __HAL_LINKDMA(SigIO_TIM, hdma[TIM_DMA_ID_UPDATE], SigIO_DMA_DAC);

  SigIO_DMA_ADC.Instance = DMA2_Stream0;
  SigIO_DMA_ADC.Init.Channel = DMA_CHANNEL_0;
  SigIO_DMA_ADC.Init.Direction = DMA_PERIPH_TO_MEMORY;
  SigIO_DMA_ADC.Init.PeriphInc = DMA_PINC_DISABLE;
  SigIO_DMA_ADC.Init.MemInc = DMA_MINC_ENABLE;
  SigIO_DMA_ADC.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  SigIO_DMA_ADC.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  SigIO_DMA_ADC.Init.Mode = DMA_CIRCULAR;
  SigIO_DMA_ADC.Init.Priority = DMA_PRIORITY_LOW;
  SigIO_DMA_ADC.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

  HAL_DMA_DeInit(&SigIO_DMA_ADC);
  HAL_DMA_Init(&SigIO_DMA_ADC);

  __HAL_LINKDMA(SigIO_ADC, DMA_Handle, SigIO_DMA_ADC);

  SigIO_ADC->Instance->CR2 |= ADC_CR2_DDS;

  __HAL_TIM_ENABLE_DMA(SigIO_TIM, TIM_DMA_UPDATE);

  SigIO_IT_Init();
}

void SigIO_IT_Init()
{
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

  __HAL_DMA_CLEAR_FLAG(&SigIO_DMA_ADC, __HAL_DMA_GET_TC_FLAG_INDEX(&SigIO_DMA_ADC));
  SigIO_DMA_ADC.Instance->CR |= DMA_IT_TC;
  __HAL_DMA_CLEAR_FLAG(&SigIO_DMA_DAC, __HAL_DMA_GET_TC_FLAG_INDEX(&SigIO_DMA_DAC));
  SigIO_DMA_DAC.Instance->CR |= DMA_IT_TC;
}

void SigIO_Start(SigIO_TargetType *bufAddr, uint32_t partLen)
{
  SigIO_BaseBufAddr = bufAddr;
  SigIO_PartLen = partLen;
  //Extracted from HAL_ADC_Start_DMA()
  SigIO_ADC->Instance->CR2 |= ADC_CR2_DMA;
  HAL_ADC_Start(SigIO_ADC);
  nextADCPtr = 0;
  nextDACPtr = 1;
  HAL_DMAEx_MultiBufferStart(&SigIO_DMA_ADC, (uint32_t)(&(ADC1->DR)), (uint32_t)(bufAddr + 0 * partLen), (uint32_t)(bufAddr + 1 * partLen), partLen);
  HAL_DMAEx_MultiBufferStart(&SigIO_DMA_DAC, (uint32_t)(bufAddr + 1 * partLen), (uint32_t)(&(DAC->DHR12R2)), (uint32_t)(bufAddr + 2 * partLen), partLen);
  HAL_TIM_Base_Start(SigIO_TIM);
}

void DMA1_Stream1_IRQHandler()
{
  __HAL_DMA_CLEAR_FLAG(&SigIO_DMA_DAC, __HAL_DMA_GET_TC_FLAG_INDEX(&SigIO_DMA_DAC));
  nextDACPtr += 1;
  nextDACPtr %= 3;
  // Switched from Memory1 to Memory0, Memory1 is accessible
  if ((SigIO_DMA_DAC.Instance->CR & DMA_SxCR_CT) == RESET)
    HAL_DMAEx_ChangeMemory(&SigIO_DMA_DAC, (uint32_t)(SigIO_BaseBufAddr + nextDACPtr * SigIO_PartLen), MEMORY1);
  else
    HAL_DMAEx_ChangeMemory(&SigIO_DMA_DAC, (uint32_t)(SigIO_BaseBufAddr + nextDACPtr * SigIO_PartLen), MEMORY0);
}

void DMA2_Stream0_IRQHandler()
{
  SigIO_TargetType *procPtr;

  __HAL_DMA_CLEAR_FLAG(&SigIO_DMA_ADC, __HAL_DMA_GET_TC_FLAG_INDEX(&SigIO_DMA_ADC));
  nextADCPtr += 1;
  nextADCPtr %= 3;
  // Switched from Memory1 to Memory0, Memory1 is accessible
  if ((SigIO_DMA_ADC.Instance->CR & DMA_SxCR_CT) == RESET)
    HAL_DMAEx_ChangeMemory(&SigIO_DMA_ADC, (uint32_t)(SigIO_BaseBufAddr + nextADCPtr * SigIO_PartLen), MEMORY1);
  else
    HAL_DMAEx_ChangeMemory(&SigIO_DMA_ADC, (uint32_t)(SigIO_BaseBufAddr + nextADCPtr * SigIO_PartLen), MEMORY0);

  if (SigIO_ProcFunc != NULL)
  {
    procPtr = SigIO_BaseBufAddr + ((nextADCPtr + 1) % 3) * SigIO_PartLen;
    SigIO_ProcFunc(procPtr, SigIO_PartLen);
  }
}
