#include "ad9833.h"

void AD9833_Stop()
{
  
}

void AD9833_SendRaw(uint16_t data)
{
  HAL_SPI_Transmit(&hspi2,(uint8_t*)&data,1,100);
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