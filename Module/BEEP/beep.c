#include "beep.h"

TIM_HandleTypeDef *BeepTIMHandle = &htim3;
uint32_t BeepTIMChannel = TIM_CHANNEL_1;
uint16_t BeepTIMClkFreq;
uint16_t BeepSYSClkFreq;
uint8_t BeepMode = 0; // 0:TIM and PWM, 1:Delay

float freqTable[12][10] = {
    // note: C, C#/Db, D, D#/Eb, E, F, F#/Gb, G, G#/Ab, A, A#/Bb, B
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

void Beep_Init_TIM(TIM_HandleTypeDef *TIMHandle, uint32_t TIMChannel, uint16_t TIMFreq)
{
  BeepTIMHandle = TIMHandle;
  BeepTIMChannel = TIMChannel;
  Beep_SetTIMClkFreq(TIMFreq);
  HAL_TIM_PWM_Init(BeepTIMHandle);
  HAL_TIM_PWM_Start(BeepTIMHandle, TIMChannel);
  // __HAL_TIM_DISABLE(); doesn't work there
  BeepTIMHandle->Instance->CR1 &= ~(TIM_CR1_CEN);
  BeepMode = 0;
}

void Beep_Init_Delay(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  BEEP_GPIO_CLKEN();

  GPIO_InitStruct.Pin = BEEP_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(BEEP_GPIO, &GPIO_InitStruct);

  BeepMode = 1;
}

void Beep_Beep(uint8_t note, uint8_t octave, uint16_t duration) // duration in ms
{
  if(note>=12 || octave>=10)
  {
    Delay_ms(duration);
    return;
  }
  if (BeepMode == 0) // PWM Mode
  {
    uint32_t TVal;
    uint16_t arrVal, pscVal;
    TVal = BeepTIMClkFreq * 1000000;
    TVal /= freqTable[note][octave];
    arrVal = TVal / 65534 + 1;
    if (arrVal & 1u) // make arrVal even
      arrVal++;
    pscVal = TVal / arrVal;
    Beep_SetTIMPara(arrVal - 1, pscVal - 1, arrVal / 2);
    __HAL_TIM_ENABLE(BeepTIMHandle);
    Delay_ms(duration);
    // __HAL_TIM_DISABLE(); doesn't work there
    BeepTIMHandle->Instance->CR1 &= ~(TIM_CR1_CEN);
  }
  else // Delay Mode
  {
    uint32_t duraTicks, tuneTicks, counter;
    duraTicks = Delay_GetSYSFreq() / 1000 * duration;
    tuneTicks = Delay_GetSYSFreq() / 2 / freqTable[note][octave];
    counter = 0;
    while (counter <= duraTicks)
    {
      Delay_ticks(tuneTicks);
      HAL_GPIO_WritePin(BEEP_GPIO, BEEP_PIN, 0);
      Delay_ticks(tuneTicks);
      HAL_GPIO_WritePin(BEEP_GPIO, BEEP_PIN, 1);
      counter += 2 * tuneTicks;
    }
  }
}