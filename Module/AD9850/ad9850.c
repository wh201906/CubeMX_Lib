#include "ad9850.h"

#define MODE_SERIAL 1

#define FQUD(x) (HAL_GPIO_WritePin(AD9850_FQUD_GPIO, AD9850_FQUD_PIN, x))
#define RESET(x) (HAL_GPIO_WritePin(AD9850_RESET_GPIO, AD9850_RESET_PIN, x))
#define WCLK(x) (HAL_GPIO_WritePin(AD9850_WCLK_GPIO, AD9850_WCLK_PIN, x))
#define D7(x) (HAL_GPIO_WritePin(AD9850_D_GPIO, AD9850_D7_PIN, x))

uint32_t freqReg = 0;
uint8_t phaseAndPowerReg = 0;
uint8_t cmd[5] = {0};

void AD9850_ToCmdBuf(void);
void AD9850_Delay(void);
void AD9850_ModeSelect(void);

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
  AD9850_ToCmdBuf();
  AD9850_SendRaw(cmd);
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
  return phaseAndPowerReg;
}

void AD9850_SetPhase(double phase)
{
  phaseAndPowerReg &= 0x07;
  phaseAndPowerReg |= AD9850_Phase2Reg(phase);
  AD9850_ToCmdBuf();
  AD9850_SendRaw(cmd);
}

void AD9850_SendRaw(uint8_t *data)
{
  uint8_t i, j;
  FQUD(0);
#if MODE_SERIAL
  AD9850_Delay();
  FQUD(1);
  AD9850_Delay();
  FQUD(0);
  for (i = 0; i < 5; i++)
    for (j = 0; j < 8; j++)
    {
      D7((data[i] >> j) & 1u);
      WCLK(1);
      AD9850_Delay();
      WCLK(0);
    }
#else
  for(i = 0;i<5;i++)
  {
    AD9850_D_GPIO->ODR=(data[i]<<8); // if use [7:0], remove the Lshift
    WCLK(1);
    AD9850_Delay();
    WCLK(0);
    AD9850_Delay();
  }
#endif
  FQUD(1);
  AD9850_Delay();
  FQUD(0);
}

void AD9850_SetPowerDown(uint8_t isPowerDown)
{
  phaseAndPowerReg &= 0xF8;
  if (isPowerDown)
    phaseAndPowerReg |= 0x04;
  AD9850_ToCmdBuf();
  AD9850_SendRaw(cmd);
}

void AD9850_Reset(void)
{
  uint8_t i;
  RESET(1);
  Delay_us(1);
  RESET(0);
  AD9850_ModeSelect();
}

void AD9850_ToCmdBuf(void)
{
  cmd[0] = freqReg >> 0;
  cmd[1] = freqReg >> 8;
  cmd[2] = freqReg >> 16;
  cmd[3] = freqReg >> 24;
  cmd[4] = phaseAndPowerReg;
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

  HAL_GPIO_WritePin(AD9850_FQUD_GPIO, AD9850_FQUD_PIN, 0);
  GPIO_InitStruct.Pin = AD9850_FQUD_PIN;
  HAL_GPIO_Init(AD9850_FQUD_GPIO, &GPIO_InitStruct);

  HAL_GPIO_WritePin(AD9850_RESET_GPIO, AD9850_RESET_PIN, 0);
  GPIO_InitStruct.Pin = AD9850_RESET_PIN;
  HAL_GPIO_Init(AD9850_RESET_GPIO, &GPIO_InitStruct);

  HAL_GPIO_WritePin(AD9850_WCLK_GPIO, AD9850_WCLK_PIN, 0);
  GPIO_InitStruct.Pin = AD9850_WCLK_PIN;
  HAL_GPIO_Init(AD9850_WCLK_GPIO, &GPIO_InitStruct);
  
  AD9850_ModeSelect();
}

void AD9850_Delay(void)
{
  // 2ns for single __nop(); at 480MHz, FQUD should be high for at least 7ns
  __NOP();
  __NOP();
  __NOP();
  __NOP();
}

void AD9850_ModeSelect(void)
{
  // see AD9850 datasheet, Rev.H, Figure 10
  WCLK(0);
  FQUD(0);
#if MODE_SERIAL
  HAL_GPIO_WritePin(AD9850_D_GPIO,AD9850_D0_PIN,1);
  HAL_GPIO_WritePin(AD9850_D_GPIO,AD9850_D1_PIN,1);
  HAL_GPIO_WritePin(AD9850_D_GPIO,AD9850_D2_PIN,0);
#endif
  AD9850_Delay();
  WCLK(1);
  AD9850_Delay();
  WCLK(0);
  AD9850_Delay();
  FQUD(1);
  AD9850_Delay();
  FQUD(0);
}