#include "ad9834.h"

uint16_t AD9834_ctrlReg = AD9834_REGADDR_CTRL;
uint32_t AD9834_freqReg[2] = {AD9834_REGADDR_FREQ0, AD9834_REGADDR_FREQ1};
uint16_t AD9834_phaseReg[2] = {AD9834_REGADDR_PHASE0, AD9834_REGADDR_PHASE1};

SPI_HandleTypeDef *AD9834_hspi;

void AD9834_SetWaveType(AD9834_IoutWaveType IoutType, AD9834_SoutWaveType SoutType)
{
  AD9834_ctrlReg &= ~AD9834_WAVE_MASK;

  if (IoutType == AD9834_Tri) // ignore SoutType
  {
    AD9834_ctrlReg |= AD9834_WAVE_SQOFF;
    AD9834_ctrlReg |= AD9834_WAVE_TRI;
  }
  else
  {
    if (SoutType == AD9834_SOff)
      AD9834_ctrlReg |= AD9834_WAVE_SQOFF;
    else if (SoutType == AD9834_Square)
      AD9834_ctrlReg |= AD9834_WAVE_SQUARE;
    else if (SoutType == AD9834_Square2)
      AD9834_ctrlReg |= AD9834_WAVE_SQUARE2;
    else if (SoutType == AD9834_Square_Comp)
      AD9834_ctrlReg |= AD9834_WAVE_SQUARE_COMP;

    if (IoutType == AD9834_IOff)
      AD9834_ctrlReg |= AD9834_WAVE_DACOFF;
    else if (IoutType == AD9834_Sine)
      AD9834_ctrlReg |= AD9834_WAVE_SINE;
  }

  AD9834_SendRaw(AD9834_ctrlReg);
}

void AD9834_SelectReg(uint8_t freqRegID, uint8_t phaseRegID)
{
  AD9834_ctrlReg &= ~AD9834_REGSEL_MASK;
  if (freqRegID)
    AD9834_ctrlReg |= AD9834_REGSEL_F1;
  if (phaseRegID)
    AD9834_ctrlReg |= AD9834_REGSEL_P1;
  AD9834_SendRaw(AD9834_ctrlReg);
}

void AD9834_SetFreqConfMode(AD9834_FreqConfMode mode)
{
  AD9834_ctrlReg &= ~AD9834_FREQREG_MASK;
  if (mode == AD9834_LSB)
    AD9834_ctrlReg |= AD9834_FREQREG_LSB;
  else if (mode == AD9834_MSB)
    AD9834_ctrlReg |= AD9834_FREQREG_MSB;
  else if (mode == AD9834_Full)
    AD9834_ctrlReg |= AD9834_FREQREG_FULL;
  AD9834_SendRaw(AD9834_ctrlReg);
}

uint32_t AD9834_Freq2Reg(double freq, uint8_t regID)
{
  uint32_t res;
  res = POW2_28 / (double)AD9834_CLK * freq + 0.5;
  res = (res & 0x3FFFu) | (res << 2 & 0x3FFF0000u);
  regID++; // 00/01 -> 01/10
  res |= (regID << 14);
  res |= (regID << 30);
  return res;
}

double AD9834_GetActuralFreq(uint32_t regVal)
{
  double res;
  regVal = (regVal & 0x3FFFu) | (regVal >> 2 & 0x0FFFC000u);
  regVal &= 0x0FFFFFFFu;
  res = regVal * AD9834_CLK / POW2_28;
  return res;
}

void AD9834_SetFreq(double freq, uint8_t regID)
{
  uint16_t tempReg;
  AD9834_freqReg[regID] = AD9834_Freq2Reg(freq, regID);
  tempReg = AD9834_ctrlReg & ~AD9834_FREQREG_MASK;
  tempReg |= AD9834_FREQREG_FULL;

  AD9834_SendRaw(tempReg); // Force Full write
  AD9834_SendRaw((uint16_t)(AD9834_freqReg[regID] >> 0));
  AD9834_SendRaw((uint16_t)(AD9834_freqReg[regID] >> 16));

  if (tempReg != AD9834_ctrlReg)
    AD9834_SendRaw(AD9834_ctrlReg); // Restore the FreqConfMode
}

void AD9834_SetFreqMSB(double freq, uint8_t regID)
{
  AD9834_freqReg[regID] &= 0x0000FFFFu;
  AD9834_freqReg[regID] |= (AD9834_Freq2Reg(freq, regID) & 0xFFFF0000u);
  AD9834_SendRaw(AD9834_ctrlReg);
  AD9834_SendRaw((uint16_t)(AD9834_freqReg[regID] >> 16));
}

void AD9834_SetFreqLSB(double freq, uint8_t regID)
{
  AD9834_freqReg[regID] &= 0xFFFF0000u;
  AD9834_freqReg[regID] |= (AD9834_Freq2Reg(freq, regID) & 0x0000FFFFu);
  AD9834_SendRaw(AD9834_ctrlReg); // Force Full write
  AD9834_SendRaw((uint16_t)(AD9834_freqReg[regID] >> 0));
}

uint16_t AD9834_Phase2Reg(double phase, uint8_t regID)
{
  uint16_t res;
  res = AD9834_PHASE_DELTA * phase + 0.5;
  res &= 0x0FFFu;
  regID++; // 00/01 -> 01/10 (0/1, left bit)
  res |= (regID << 12);
  return res;
}

double AD9834_GetActuralPhase(uint16_t regVal)
{
  double res;
  regVal &= 0x0FFFu;
  res = regVal * AD9834_PHASE_DELTA;
  return res;
}

void AD9834_SetPhase(double phase, uint8_t regID)
{
  AD9834_phaseReg[regID] = AD9834_Phase2Reg(phase, regID);
  AD9834_SendRaw(AD9834_phaseReg[regID]);
}

void AD9834_Init(SPI_HandleTypeDef *hspi)
{
  AD9834_NSS_CLKEN();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = AD9834_NSS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(AD9834_NSS_GPIO, &GPIO_InitStruct);
  HAL_GPIO_WritePin(AD9834_NSS_GPIO, AD9834_NSS_PIN, 1);

  AD9834_hspi = hspi;

  AD9834_SetWaveType(AD9834_Sine, AD9834_SOff);
  AD9834_SetRegSelSrc(0);
  AD9834_SelectReg(0, 0);
  AD9834_SetFreqConfMode(AD9834_Full);
}

void AD9834_SendRaw(uint16_t data)
{
  HAL_GPIO_WritePin(AD9834_NSS_GPIO, AD9834_NSS_PIN, 0);
  HAL_SPI_Transmit(AD9834_hspi, (uint8_t *)(&data), 1, 10);
  HAL_GPIO_WritePin(AD9834_NSS_GPIO, AD9834_NSS_PIN, 1);
}

uint32_t AD9834_GetCurrentFreqReg(uint8_t regID)
{
  return AD9834_freqReg[regID];
}

uint16_t AD9834_GetCurrentPhaseReg(uint8_t regID)
{
  return AD9834_phaseReg[regID];
}

void AD9834_SetResetState(uint8_t isReset)
{
  if (isReset)
    AD9834_ctrlReg |= AD9834_RESET;
  else
    AD9834_ctrlReg &= ~AD9834_RESET;
  AD9834_SendRaw(AD9834_ctrlReg);
}

void AD9834_SetRegSelSrc(uint8_t isHw)
{
  if (isHw)
    AD9834_ctrlReg |= AD9834_REGSEL_HW;
  else
    AD9834_ctrlReg &= ~AD9834_REGSEL_HW;
  AD9834_SendRaw(AD9834_ctrlReg);
}