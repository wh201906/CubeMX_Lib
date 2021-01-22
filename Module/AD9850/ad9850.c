#include "ad9850.h"

#define MODE_SERIAL 1

#define FQUD(x) (HAL_GPIO_WritePin(AD9850_FQUD_GPIO, AD9850_FQUD_PIN, x))
#define RESET(x) (HAL_GPIO_WritePin(AD9850_RESET_GPIO, AD9850_RESET_PIN, x))
#define WCLK(x) (HAL_GPIO_WritePin(AD9850_WCLK_GPIO, AD9850_WCLK_PIN, x))
#define D7(x) (HAL_GPIO_WritePin(AD9850_D_GPIO, AD9850_D7_PIN, x))

uint32_t freqReg = 0;
uint8_t phaseReg = 0;

void AD9850_ToCmdBuf(void);
void AD9850_Delay(void);
void AD9850_ModeUpdate(void);
void AD9850_WCLKPulse(void);
void AD9850_FQUDPulse(void);

uint32_t AD9850_Freq2Reg(double freq)
{
  return (POW2_32 / (double)AD9850_CLK * freq + 0.5);
}

double AD9850_GetActuralFreq(uint32_t regVal)
{
  return (regVal * AD9850_CLK / POW2_32);
}

uint32_t AD9850_GetCurrentFreqReg(void)
{
  return freqReg;
}

void AD9850_SetFreq(double freq)
{
  freqReg = AD9850_Freq2Reg(freq);
  AD9850_Update();
}

uint8_t AD9850_Phase2Reg(double phase)
{
  uint8_t res;
  res = AD9850_PHASE_DELTA * phase + 0.5;
  res &= 0x1F;
  res <<= 3;
  return res;
}

double AD9850_GetActuralPhase(uint8_t regVal)
{
  double res;
  regVal >>= 3;
  regVal &= 0x1F;
  res = regVal * AD9850_PHASE_DELTA;
  return res;
}

uint8_t AD9850_GetCurrentPhaseReg()
{
  return phaseReg;
}

void AD9850_SetPhase(double phase)
{
  phaseReg = AD9850_Phase2Reg(phase);
  AD9850_Update();
}

void AD9850_Update(void) // An update with W34=0 will power up the chip
{
  // reset register pointer
  AD9850_FQUDPulse();

  AD9850_SendByte(freqReg >> 0);
  AD9850_SendByte(freqReg >> 8);
  AD9850_SendByte(freqReg >> 16);
  AD9850_SendByte(freqReg >> 24);
  AD9850_SendByte(phaseReg);

  AD9850_FQUDPulse();
}

void AD9850_SetPowerDown(uint8_t isPowerDown)
{
  if(isPowerDown)
  {
    AD9850_FQUDPulse();
    AD9850_SendByte(0x04);
    AD9850_FQUDPulse();
  }
  else
    AD9850_Update(); // An update with W34=0 will power up the chip
}

void AD9850_Reset(void)
{
  RESET(1);
  Delay_us(1);
  RESET(0);
}

void AD9850_Init()
{
  uint16_t dataGPIOPins = AD9850_D0_PIN |
                          AD9850_D1_PIN |
                          AD9850_D2_PIN |
                          AD9850_D3_PIN |
                          AD9850_D4_PIN |
                          AD9850_D5_PIN |
                          AD9850_D6_PIN |
                          AD9850_D7_PIN;

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  HAL_GPIO_WritePin(AD9850_D_GPIO, dataGPIOPins, 0);
  GPIO_InitStruct.Pin = dataGPIOPins;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(AD9850_D_GPIO, &GPIO_InitStruct);

#if MODE_SERIAL
  HAL_GPIO_WritePin(AD9850_D_GPIO, AD9850_D0_PIN | AD9850_D1_PIN, 1);
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(AD9850_D_GPIO, &GPIO_InitStruct);
#endif

  HAL_GPIO_WritePin(AD9850_FQUD_GPIO, AD9850_FQUD_PIN, 0);
  GPIO_InitStruct.Pin = AD9850_FQUD_PIN;
  HAL_GPIO_Init(AD9850_FQUD_GPIO, &GPIO_InitStruct);

  HAL_GPIO_WritePin(AD9850_RESET_GPIO, AD9850_RESET_PIN, 0);
  GPIO_InitStruct.Pin = AD9850_RESET_PIN;
  HAL_GPIO_Init(AD9850_RESET_GPIO, &GPIO_InitStruct);

  HAL_GPIO_WritePin(AD9850_WCLK_GPIO, AD9850_WCLK_PIN, 0);
  GPIO_InitStruct.Pin = AD9850_WCLK_PIN;
  HAL_GPIO_Init(AD9850_WCLK_GPIO, &GPIO_InitStruct);

  AD9850_ModeUpdate();
}

void AD9850_Delay(void)
{
  // 2ns for single __nop(); at 480MHz, FQUD should be high for at least 7ns
  __NOP();
  __NOP();
  __NOP();
  __NOP();
}

void AD9850_ModeUpdate(void)
{
  // see AD9850 datasheet, Rev.H, Figure 10
  AD9850_FQUDPulse();
  AD9850_WCLKPulse();
  AD9850_FQUDPulse();
}

void AD9850_WCLKPulse(void)
{
  WCLK(1);
  AD9850_Delay();
  WCLK(0);
  AD9850_Delay();
}

void AD9850_FQUDPulse(void)
{
  FQUD(1);
  AD9850_Delay();
  FQUD(0);
  AD9850_Delay();
}

void AD9850_SendByte(uint8_t data)
{
#if MODE_SERIAL
  uint8_t j;
  for (j = 0; j < 8; j++)
  {
    D7((data >> j) & 1u);
    AD9850_WCLKPulse();
  }
#else
  AD9850_D_GPIO->ODR = (data << 8); // if use [7:0], remove the Lshift
  AD9850_WCLKPulse();
#endif
}