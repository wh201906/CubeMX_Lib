#include "ad9833.h"

uint16_t ctrlReg=AD9833_REGADDR_CTRL;
uint32_t freqReg[2]={AD9833_REGADDR_FREQ0,AD9833_REGADDR_FREQ1};
uint16_t phaseReg[2]={AD9833_REGADDR_PHASE0,AD9833_REGADDR_PHASE1};

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

void AD9833_SelectReg(uint8_t freqRegID,uint8_t phaseRegID)
{
  ctrlReg&=~AD9833_REGSEL_MASK;
  if(freqRegID)
    ctrlReg|=AD9833_REGSEL_F1;
  if(phaseRegID)
    ctrlReg|=AD9833_REGSEL_P1;
  AD9833_SendRaw(ctrlReg);
}

void AD9833_SetFreqConfMode(AD9833_FreqConfMode mode)
{
  ctrlReg&=~AD9833_FREQREG_MASK;
  if(mode==AD9833_LSB)
    ctrlReg|=AD9833_FREQREG_LSB;
  else if(mode==AD9833_MSB)
    ctrlReg|=AD9833_FREQREG_MSB;
  else if(mode==AD9833_Full)
    ctrlReg|=AD9833_FREQREG_FULL;
  AD9833_SendRaw(ctrlReg);
}


uint32_t AD9833_Freq2Reg(double freq, uint8_t regID)
{
  uint32_t res;
  res=POW2_28/(double)AD9833_CLK*freq+0.5;
  res=(res&0x3FFFu)|(res<<2&0x3FFF0000u);
  regID++; // 00/01 -> 01/10
  res|=(regID<<14);
  res|=(regID<<30);
  return res;
}

double AD9833_GetActuralFreq(uint32_t regVal)
{
  double res;
  regVal=(regVal&0x3FFFu)|(regVal>>2&0x0FFFC000u);
  regVal&=0x0FFFFFFFu;
  res=regVal*AD9833_CLK/POW2_28;
  return res;
}

void AD9833_SetFreq(double freq, uint8_t regID)
{
  uint16_t tempReg;
  freqReg[regID]=AD9833_Freq2Reg(freq,regID);
  tempReg=ctrlReg&~AD9833_FREQREG_MASK;
  tempReg|=AD9833_FREQREG_FULL;
  
  AD9833_SendRaw(tempReg); // Force Full write
  AD9833_SendRaw((uint16_t)(freqReg[regID]>>0));
  AD9833_SendRaw((uint16_t)(freqReg[regID]>>16));
  
  if(tempReg!=ctrlReg)
    AD9833_SendRaw(ctrlReg); // Restore the FreqConfMode
}

void AD9833_SetFreqMSB(double freq, uint8_t regID)
{
  freqReg[regID]&=0x0000FFFFu;
  freqReg[regID]|=(AD9833_Freq2Reg(freq,regID)&0xFFFF0000u);
  AD9833_SendRaw(ctrlReg);
  AD9833_SendRaw((uint16_t)(freqReg[regID]>>16));
}

void AD9833_SetFreqLSB(double freq, uint8_t regID)
{
  freqReg[regID]&=0xFFFF0000u;
  freqReg[regID]|=(AD9833_Freq2Reg(freq,regID)&0x0000FFFFu);
  AD9833_SendRaw(ctrlReg); // Force Full write
  AD9833_SendRaw((uint16_t)(freqReg[regID]>>0));
}

uint16_t AD9833_Phase2Reg(double phase, uint8_t regID)
{
  uint16_t res;
  res=11.377777777777778*phase+0.5;
  res&=0x0FFF;
  regID++; // 00/01 -> 01/10 (0/1, left bit)
  res|=(regID<<12);
  return res;
}

double AD9833_GetActuralPhase(uint16_t regVal)
{
  double res;
  regVal&=0x0FFFu;
  res=regVal*11.377777777777778;
  return res;
}

void AD9833_SetPhase(double phase, uint8_t regID)
{
  phaseReg[regID]=AD9833_Phase2Reg(phase,regID);
  AD9833_SendRaw(phaseReg[regID]);
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
  
  AD9833_SetWaveType(AD9833_Sine);
  AD9833_SelectReg(0,0);
  AD9833_SetFreqConfMode(AD9833_Full);
  
}

void AD9833_SendRaw(uint16_t data)
{
  HAL_GPIO_WritePin(AD9833_NSS_GPIO,AD9833_NSS_PIN,0);
  HAL_SPI_Transmit(&hspi2,(uint8_t*)(&data),1,10);
  HAL_GPIO_WritePin(AD9833_NSS_GPIO,AD9833_NSS_PIN,1);
}

uint32_t AD9833_GetCurrentFreqReg(uint8_t regID)
{
  return freqReg[regID];
}

uint16_t AD9833_GetCurrentPhaseReg(uint8_t regID)
{
  return phaseReg[regID];
}

void AD9833_SetResetState(uint8_t isReset)
{
  if(isReset)
    ctrlReg|=AD9833_RESET;
  else
    ctrlReg&=~AD9833_RESET;
  AD9833_SendRaw(ctrlReg);
}
