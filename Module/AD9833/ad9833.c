#include "ad9833.h"

uint16_t AD9833_ctrlReg = AD9833_REGADDR_CTRL;
uint32_t AD9833_freqReg[2] = {AD9833_REGADDR_FREQ0, AD9833_REGADDR_FREQ1};
uint16_t AD9833_phaseReg[2] = {AD9833_REGADDR_PHASE0, AD9833_REGADDR_PHASE1};

SPI_HandleTypeDef *AD9833_hspi;

void AD9833_SetWaveType(AD9833_WaveType type)
{
  AD9833_ctrlReg &= ~AD9833_WAVE_MASK;
  if (type == AD9833_Off)
    AD9833_ctrlReg |= AD9833_WAVE_OFF;
  else if (type == AD9833_Square)
    AD9833_ctrlReg |= AD9833_WAVE_SQUARE;
  else if (type == AD9833_Square2)
    AD9833_ctrlReg |= AD9833_WAVE_SQUARE2;
  else if (type == AD9833_Sine)
    AD9833_ctrlReg |= AD9833_WAVE_SINE;
  else if (type == AD9833_Tri)
    AD9833_ctrlReg |= AD9833_WAVE_TRI;
  AD9833_SendRaw(AD9833_ctrlReg);
}

void AD9833_SelectReg(uint8_t freqRegID, uint8_t phaseRegID)
{
  AD9833_ctrlReg &= ~AD9833_REGSEL_MASK;
  if (freqRegID)
    AD9833_ctrlReg |= AD9833_REGSEL_F1;
  if (phaseRegID)
    AD9833_ctrlReg |= AD9833_REGSEL_P1;
  AD9833_SendRaw(AD9833_ctrlReg);
}

void AD9833_SetFreqConfMode(AD9833_FreqConfMode mode)
{
  AD9833_ctrlReg &= ~AD9833_FREQREG_MASK;
  if (mode == AD9833_LSB)
    AD9833_ctrlReg |= AD9833_FREQREG_LSB;
  else if (mode == AD9833_MSB)
    AD9833_ctrlReg |= AD9833_FREQREG_MSB;
  else if (mode == AD9833_Full)
    AD9833_ctrlReg |= AD9833_FREQREG_FULL;
  AD9833_SendRaw(AD9833_ctrlReg);
}

uint32_t AD9833_Freq2Reg(double freq, uint8_t regID)
{
  uint32_t res;
  res = POW2_28 / (double)AD9833_CLK * freq + 0.5;
  res = (res & 0x3FFFu) | (res << 2 & 0x3FFF0000u);
  regID++; // 00/01 -> 01/10
  res |= (regID << 14);
  res |= (regID << 30);
  return res;
}

double AD9833_GetActuralFreq(uint32_t regVal)
{
  double res;
  regVal = (regVal & 0x3FFFu) | (regVal >> 2 & 0x0FFFC000u);
  regVal &= 0x0FFFFFFFu;
  res = regVal * AD9833_CLK / POW2_28;
  return res;
}

void AD9833_SetFreq(double freq, uint8_t regID)
{
  uint16_t tempReg;
  AD9833_freqReg[regID] = AD9833_Freq2Reg(freq, regID);
  tempReg = AD9833_ctrlReg & ~AD9833_FREQREG_MASK;
  tempReg |= AD9833_FREQREG_FULL;

  AD9833_SendRaw(tempReg); // Force Full write
  AD9833_SendRaw((uint16_t)(AD9833_freqReg[regID] >> 0));
  AD9833_SendRaw((uint16_t)(AD9833_freqReg[regID] >> 16));

  if (tempReg != AD9833_ctrlReg)
    AD9833_SendRaw(AD9833_ctrlReg); // Restore the FreqConfMode
}

void AD9833_SetFreqMSB(double freq, uint8_t regID)
{
  AD9833_freqReg[regID] &= 0x0000FFFFu;
  AD9833_freqReg[regID] |= (AD9833_Freq2Reg(freq, regID) & 0xFFFF0000u);
  AD9833_SendRaw(AD9833_ctrlReg);
  AD9833_SendRaw((uint16_t)(AD9833_freqReg[regID] >> 16));
}

void AD9833_SetFreqLSB(double freq, uint8_t regID)
{
  AD9833_freqReg[regID] &= 0xFFFF0000u;
  AD9833_freqReg[regID] |= (AD9833_Freq2Reg(freq, regID) & 0x0000FFFFu);
  AD9833_SendRaw(AD9833_ctrlReg); // Force Full write
  AD9833_SendRaw((uint16_t)(AD9833_freqReg[regID] >> 0));
}

uint16_t AD9833_Phase2Reg(double phase, uint8_t regID)
{
  uint16_t res;
  res = AD9833_PHASE_DELTA * phase + 0.5;
  res &= 0x0FFFu;
  regID++; // 00/01 -> 01/10 (0/1, left bit)
  res |= (regID << 12);
  return res;
}

double AD9833_GetActuralPhase(uint16_t regVal)
{
  double res;
  regVal &= 0x0FFFu;
  res = regVal * AD9833_PHASE_DELTA;
  return res;
}

void AD9833_SetPhase(double phase, uint8_t regID)
{
  AD9833_phaseReg[regID] = AD9833_Phase2Reg(phase, regID);
  AD9833_SendRaw(AD9833_phaseReg[regID]);
}

void AD9833_Init(SPI_HandleTypeDef *hspi)
{
  AD9833_NSS_CLKEN();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = AD9833_NSS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(AD9833_NSS_GPIO, &GPIO_InitStruct);
  HAL_GPIO_WritePin(AD9833_NSS_GPIO, AD9833_NSS_PIN, 1);

  AD9833_hspi = hspi;

  AD9833_SetWaveType(AD9833_Sine);
  AD9833_SelectReg(0, 0);
  AD9833_SetFreqConfMode(AD9833_Full);
}

void AD9833_SendRaw(uint16_t data)
{
  HAL_GPIO_WritePin(AD9833_NSS_GPIO, AD9833_NSS_PIN, 0);
  HAL_SPI_Transmit(AD9833_hspi, (uint8_t *)(&data), 1, 10);
  HAL_GPIO_WritePin(AD9833_NSS_GPIO, AD9833_NSS_PIN, 1);
}

uint32_t AD9833_GetCurrentFreqReg(uint8_t regID)
{
  return AD9833_freqReg[regID];
}

uint16_t AD9833_GetCurrentPhaseReg(uint8_t regID)
{
  return AD9833_phaseReg[regID];
}

void AD9833_SetResetState(uint8_t isReset)
{
  if (isReset)
    AD9833_ctrlReg |= AD9833_RESET;
  else
    AD9833_ctrlReg &= ~AD9833_RESET;
  AD9833_SendRaw(AD9833_ctrlReg);
}
