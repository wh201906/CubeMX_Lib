#include "beep.h"

TIM_HandleTypeDef *BeepTIMHandle = &htim3;
uint32_t BeepTIMChannel = TIM_CHANNEL_1;
uint16_t BeepTIMClkFreq;
uint16_t BeepSYSClkFreq;
uint8_t BeepMode = 0; // 0:TIM and PWM, 1:Delay

float freqTable[12][10] = {
    // note: C, Db, D, Eb, E, F, Gb, G, Ab, A, Bb, B
    // octave: [0,9]
    // A4=440Hz
    {16.352, 32.703, 65.406, 130.81, 261.63, 523.25, 1046.5, 2093.0, 4186.0, 8372.0},
    {17.324, 34.648, 69.296, 138.59, 277.18, 554.37, 1108.7, 2217.5, 4434.9, 8869.8},
    {18.354, 36.708, 73.416, 146.83, 293.66, 587.33, 1174.7, 2349.3, 4698.6, 9397.3},
    {19.445, 38.891, 77.782, 155.56, 311.13, 622.25, 1244.5, 2489.0, 4978.0, 9956.1},
    {20.602, 41.203, 82.407, 164.81, 329.63, 659.26, 1318.5, 2637.0, 5274.0, 10548},
    {21.827, 43.654, 87.307, 174.61, 349.23, 698.46, 1396.9, 2793.8, 5587.7, 11175},
    {23.125, 46.249, 92.499, 185.00, 369.99, 739.99, 1480.0, 2960.0, 5919.9, 11840},
    {24.500, 48.999, 97.999, 196.00, 392.00, 783.99, 1568.0, 3136.0, 6271.9, 12544},
    {25.957, 51.913, 103.83, 207.65, 415.30, 830.61, 1661.2, 3322.4, 6644.9, 13290},
    {27.500, 55.000, 110.00, 220.00, 440.00, 880.00, 1760.0, 3520.0, 7040.0, 14080},
    {29.135, 58.270, 116.54, 233.08, 466.16, 932.33, 1864.7, 3729.3, 7458.6, 14917},
    {30.868, 61.735, 123.47, 246.94, 493.88, 987.77, 1975.5, 3951.1, 7902.1, 15804},
};

void Beep_SetTIMPara(uint32_t arrVal, uint16_t pscVal, uint32_t cmpVal)
{
  __HAL_TIM_SET_AUTORELOAD(BeepTIMHandle, arrVal);
  __HAL_TIM_SET_PRESCALER(BeepTIMHandle, pscVal);
  __HAL_TIM_SET_COMPARE(BeepTIMHandle, BeepTIMChannel, cmpVal);
}

void Beep_SetTIMClkFreq(uint16_t freq)
{
  BeepTIMClkFreq = freq;
}

void Beep_SetSYSClkFreq(uint16_t freq)
{
  BeepSYSClkFreq = freq;
}

void Beep_Init_TIM(TIM_HandleTypeDef *TIMHandle, uint32_t TIMChannel, uint16_t TIMFreq)
{
  BeepTIMHandle = TIMHandle;
  BeepTIMChannel = TIMChannel;
  Beep_SetTIMClkFreq(TIMFreq);
  BeepMode = 0;
}

void Beep_Init_Delay(uint16_t sysFreq)
{
  Beep_SetSYSClkFreq(sysFreq);
  BeepMode = 1;
}

void Beep_DelayTicks(uint32_t ticks)
{
  uint32_t told, tnow, tcnt = 0;
  uint32_t reload = SysTick->LOAD;
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
  };
}

void Beep_Beep(uint8_t note, uint8_t octave, uint16_t duration) // duration in ms
{
}