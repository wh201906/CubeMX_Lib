#ifndef _MOD_H
#define _MOD_H

#include "main.h"

#define SIGPARA_HTIM1_CLK 84000000.0

typedef struct _Mod_Tx_Dev
{
  DMA_HandleTypeDef DMAHandle;
  TIM_HandleTypeDef *htim;
  uint16_t data;

  uint32_t comp0;
  uint32_t comp1;
} Mod_Tx_Dev;

void Mod_Tx_DMAInit(Mod_Tx_Dev *dev, DMA_Stream_TypeDef *DMAStream, uint32_t DMAChannel, IRQn_Type DMAIRQ);
void Mod_Tx_Init(Mod_Tx_Dev *dev, DMA_Stream_TypeDef *DMAStream, uint32_t DMAChannel, IRQn_Type DMAIRQ, TIM_HandleTypeDef *htim);
void Mod_Tx_SetValue(Mod_Tx_Dev *dev, uint16_t data);
void Mod_Tx_Start(Mod_Tx_Dev *dev, uint32_t TIMChannel);
void Mod_Tx_UpdateBuf(void);
uint8_t Mod_Tx_IsFinished(void);

static void SigPara_Freq_LF_GPIO_Init(void);
static void SigPara_PWM_TIM_Init(void);
static void SigPara_PWM_GPIO_Init(void);
void SigPara_PWM_Init(void);
void SigPara_PWM();

#endif