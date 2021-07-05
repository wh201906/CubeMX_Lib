#include "servo.h"

void Servo_Init(ServoHandle *handle, TIM_HandleTypeDef *htim, uint32_t Channel, uint16_t initPos)
{
  handle->htim = htim;
  handle->Channel = Channel;
  handle->delay = 10000;
  handle->htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  handle->htim->Instance->CR1 |= TIM_CR1_ARPE;
  __HAL_TIM_SET_AUTORELOAD(handle->htim, 5000); // 50ms, 20Hz
  __HAL_TIM_SET_COMPARE(handle->htim, handle->Channel, initPos);
  handle->htim->Instance->CR1 |= TIM_CR1_OPM;
}

void Servo_Start(ServoHandle *handle)
{
  HAL_TIM_OnePulse_Start(handle->htim, handle->Channel);
  Delay_ms(3000);
  __HAL_TIM_SET_AUTORELOAD(handle->htim, 20000); // 20ms, 50Hz
}

void Servo_Go(ServoHandle *handle, uint16_t pos)
{
  uint16_t currPos;
  
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