#include "delay.h"

static uint16_t sysClkFreq = 0;

void Delay_Init(uint16_t SYSCLK)
{
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  sysClkFreq = SYSCLK;
}

void Delay_us(uint32_t nus)
{
  uint32_t ticks;
  uint32_t told, tnow, tcnt = 0;
  uint32_t reload = SysTick->LOAD;
  ticks = nus * sysClkFreq; // 1MHz -> 1us
  told = SysTick->VAL;
  while (1)
  {
    tnow = SysTick->VAL;
    if (tnow != told)
    {
      if (tnow < told)
        tcnt += told - tnow;
      else
        tcnt += reload - tnow + told;
      told = tnow;
      if (tcnt >= ticks)
        break;
    }
  }
}

void Delay_ms(uint16_t nms)
{
  Delay_us(nms * 1000); // HAL_Delay() has some bugs
}

void Delay_ticks(uint32_t ticks)
{
  uint32_t told, tnow, tcnt = 0;
  uint32_t reload = SysTick->LOAD;
  told = SysTick->VAL;
  // -7 for compensation
  if (ticks < 7)
    return;
  ticks -= 7;
  while (1)
  {
    tnow = SysTick->VAL;
    if (tnow != told)
    {
      if (tnow < told)
        tcnt += told - tnow;
      else
        tcnt += reload - tnow + told + 1; // +1 for compensation
      told = tnow;
      if (tcnt >= ticks)
        break;
    }
  }
}

uint32_t Delay_GetSYSFreq(void)
{
  return sysClkFreq * 1000000;
}

// the codes of Delay_us and Delay_ticks are similar.
// Delay_us doesn't call Delay_ticks, in order to reduce a little of CPU time.