#include "ad9833.h"

uint16_t ctrlReg=AD9833_REGADDR_CTRL|AD9833_WAVE_OFF|AD9833_FREQREG_FULL|AD9833_REGSEL_F0P0;
uint32_t freq0Reg;
uint32_t freq1Reg;
uint16_t phase0Reg;
uint16_t phase1Reg;

void AD9833_SetWaveType(AD9833_WaveType type)
{
  ctrlReg&=~AD9833_WAVE_MASK;
  if(type==AD9833_Off)
    ctrlReg|=AD9833_WAVE_OFF;
  else if(type==AD9833_Square)
    ctrlReg|=AD9833_WAVE_SQUARE;
  else if(type==AD9833_Square2)
    ctrlReg|=AD9833_WAVE_SQUARE2;
  else if(type==AD9833_Sine)
    ctrlReg|=AD9833_WAVE_SINE;
  else if(type==AD9833_Tri)
    ctrlReg|=AD9833_WAVE_TRI;
  AD9833_SendRaw(ctrlReg);
}

void AD9833_SendRaw(uint16_t data)
{
  HAL_GPIO_WritePin(AD9833_NSS_GPIO,AD9833_NSS_PIN,0);
  HAL_SPI_Transmit(&hspi2,(uint8_t*)(&data),1,10);
  HAL_GPIO_WritePin(AD9833_NSS_GPIO,AD9833_NSS_PIN,1);
}

uint32_t AD9833_GetFReg(double freq, uint8_t regID)
{
  // regID->0/1
  uint32_t res;
  res=POW2_28/(double)AD9833_CLK*freq;
  res=(res&0x3FFFu)|(res<<2&0x3FFF0000u);
  regID++; // 00/01 -> 01/10
  res|=(regID<<14);
  res|=(regID<<30);
  return res;
}

void AD9833_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct={0};
  GPIO_InitStruct.Pin=AD9833_NSS_PIN;
  GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull=GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(AD9833_NSS_GPIO, &GPIO_InitStruct);
  HAL_GPIO_WritePin(AD9833_NSS_GPIO,AD9833_NSS_PIN,1);
}