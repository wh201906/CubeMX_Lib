#include "ad9850.h"

uint32_t freqReg;
uint8_t phaseAndPowerReg;
uint8_t cmd[5];

void AD9850_ToCmdBuf(void);

uint32_t AD9850_Freq2Reg(double freq)
{
  return (POW2_32/(double)AD9850_CLK*freq+0.5);
}

double AD9850_GetActuralFreq(uint32_t regVal)
{
  return (regVal*AD9850_CLK/POW2_32);
}

uint32_t AD9850_GetCurrentFreqReg(void)
{
  return freqReg;
}

void AD9850_SetFreq(double freq)
{
  freqReg=AD9850_Freq2Reg(freq);
  AD9850_ToCmdBuf();
  AD9850_SendRaw(cmd);
}

// uint8_t AD9850_Phase2Reg(double phase)
// {
//   uint16_t res;
//   res=11.377777777777778*phase+0.5;
//   res&=0x0FFF;
//   regID++; // 00/01 -> 01/10 (0/1, left bit)
//   res|=(regID<<12);
//   return res;
// }

// double AD9850_GetActuralPhase(uint16_t regVal)
// {
//   double res;
//   regVal&=0x0FFFu;
//   res=regVal*11.377777777777778;
//   return res;
// }

// uint16_t AD9850_GetCurrentPhaseReg(uint8_t regID)
// {
//   return phaseReg[regID];
// }

// void AD9850_SetPhase(double phase, uint8_t regID)
// {
//   phaseReg[regID]=AD9850_Phase2Reg(phase,regID);
//   AD9850_SendRaw(phaseReg[regID]);
// }

void AD9850_SendRaw(uint8_t* data)
{
  HAL_GPIO_WritePin(AD9850_FQUD_GPIO,AD9850_FQUD_PIN,0);
  HAL_SPI_Transmit(&hspi2,data,5,10); // 5*8=40bit
  HAL_GPIO_WritePin(AD9850_FQUD_GPIO,AD9850_FQUD_PIN,1);
  Delay_us(1);
  __NOP();
  __NOP();
  __NOP();
  __NOP(); // 2ns for single __nop(); at 480MHz, FQUD should be high for at least 7ns
  HAL_GPIO_WritePin(AD9850_FQUD_GPIO,AD9850_FQUD_PIN,0);
}

void AD9850_Reset(void)
{
  uint8_t i;
  HAL_GPIO_WritePin(AD9850_RESET_GPIO,AD9850_RESET_PIN,1);
  Delay_us(1);
  HAL_GPIO_WritePin(AD9850_RESET_GPIO,AD9850_RESET_PIN,0);
}

void AD9850_ToCmdBuf(void)
{
  cmd[0] = freqReg >> 0;
  cmd[1] = freqReg >> 8;
  cmd[2] = freqReg >> 16;
  cmd[3] = freqReg >> 24;
  cmd[4] = phaseAndPowerReg;
}
