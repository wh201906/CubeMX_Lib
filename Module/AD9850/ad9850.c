#include "ad9850.h"

#define MODE_SERIAL 0

#define FQUD(x) (HAL_GPIO_WritePin(AD9850_FQUD_GPIO, AD9850_FQUD_PIN, x))
#define RESET(x) (HAL_GPIO_WritePin(AD9850_RESET_GPIO, AD9850_RESET_PIN, x))
#define WCLK(x) (HAL_GPIO_WritePin(AD9850_WCLK_GPIO, AD9850_WCLK_PIN, x))
#define D7(x) (HAL_GPIO_WritePin(AD9850_D_GPIO, AD9850_D7_PIN, x))

uint32_t AD9850_freqReg = 0;
uint8_t AD9850_phaseReg = 0;
uint16_t AD9850_delayTicks;

// __NOP() is not recommended for delay, 
// because a single instruction will take more than one clock cycle.
// Plus, the instruction pipelining should be considered
// Delay_ticks might not be fast enough, but it's more reliable.
#define AD9850_Delay() Delay_ticks(AD9850_delayTicks)

void AD9850_ToCmdBuf(void);
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
  return AD9850_freqReg;
}

void AD9850_SetFreq(double freq)
{
  AD9850_freqReg = AD9850_Freq2Reg(freq);
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
  return AD9850_phaseReg;
}

void AD9850_SetPhase(double phase)
{
  AD9850_phaseReg = AD9850_Phase2Reg(phase);
  AD9850_Update();
}

void AD9850_Update(void) // An update with W34=0 will power up the chip
{
  // reset register pointer
  AD9850_FQUDPulse();

#if MODE_SERIAL
  AD9850_SendByte(AD9850_freqReg >> 0);
  AD9850_SendByte(AD9850_freqReg >> 8);
  AD9850_SendByte(AD9850_freqReg >> 16);
  AD9850_SendByte(AD9850_freqReg >> 24);
  AD9850_SendByte(AD9850_phaseReg);
#else
  AD9850_SendByte(AD9850_phaseReg);
  AD9850_SendByte(AD9850_freqReg >> 24);
  AD9850_SendByte(AD9850_freqReg >> 16);
  AD9850_SendByte(AD9850_freqReg >> 8);
  AD9850_SendByte(AD9850_freqReg >> 0);
#endif


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

  AD9850_RESET_CLKEN();
  AD9850_FQUD_CLKEN();
  AD9850_WCLK_CLKEN();
  AD9850_D_CLKEN();

#ifdef AD9850_FUNCPIN_CLKEN
  AD9850_FUNCPIN_CLKEN();
#endif

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

  AD9850_delayTicks = Delay_GetSYSFreq() * 0.000000007 + 1.0; // 7ns
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
  AD9850_D_GPIO->ODR = ((uint16_t)(data << 8)); // if use [7:0], remove the Lshift
  AD9850_WCLKPulse();
#endif
}