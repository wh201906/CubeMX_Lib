#include "servo.h"

void Servo_Init(ServoHandle *handle, TIM_HandleTypeDef *htim, uint32_t Channel, uint16_t initPos)
{
  handle->htim = htim;
  handle->Channel = Channel;
  handle->delay = 1000; // default delay
  handle->htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  handle->htim->Instance->CR1 |= TIM_CR1_ARPE;
  __HAL_TIM_SET_AUTORELOAD(handle->htim, 20000); // 20ms, 50Hz
  __HAL_TIM_SET_COMPARE(handle->htim, handle->Channel, __HAL_TIM_GET_AUTORELOAD(handle->htim) - initPos);
}

void Servo_Start(ServoHandle *handle)
{
  uint32_t i, delayLen;
  handle->htim->Instance->CR1 |= TIM_CR1_OPM;
  delayLen = 1000000;
  HAL_TIM_PWM_Start(handle->htim, handle->Channel);
  for (i = 0; i * delayLen < 6000000; i++) // 3s
  {
    //HAL_TIM_PWM_Start(handle->htim, handle->Channel);
    TIM_CHANNEL_STATE_SET(handle->htim, handle->Channel, HAL_TIM_CHANNEL_STATE_READY);
    Delay_us(delayLen);
  }
  handle->htim->Instance->CR1 &= ~TIM_CR1_OPM;
  HAL_TIM_PWM_Start(handle->htim, handle->Channel);
}

void Servo_Go(ServoHandle *handle, uint16_t pos)
{
  uint16_t currPos;

  pos = __HAL_TIM_GET_AUTORELOAD(handle->htim) - pos;
  currPos = __HAL_TIM_GET_COMPARE(handle->htim, handle->Channel);
  if (currPos < pos)
  {
    for (; currPos <= pos; currPos++)
    {
      Delay_us(handle->delay);
      __HAL_TIM_SET_COMPARE(handle->htim, handle->Channel, currPos);
    }
  }
  else
  {
    for (; currPos >= pos; currPos--)
    {
      Delay_us(handle->delay);
      __HAL_TIM_SET_COMPARE(handle->htim, handle->Channel, currPos);
    }
  }
}

void Servo_SetDelay(ServoHandle *handle, uint16_t delay)
{
  handle->delay = delay;
}