#include "servo.h"

void Servo_Init(ServoHandle *handle, TIM_HandleTypeDef *htim, uint32_t Channel, uint16_t initPos)
{
  handle->htim = htim;
  handle->Channel = Channel;
  handle->delay = 500; // default delay
  handle->htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  handle->htim->Instance->CR1 |= TIM_CR1_ARPE;
  __HAL_TIM_SET_AUTORELOAD(handle->htim, 20000); // 20ms, 50Hz
  __HAL_TIM_SET_COMPARE(handle->htim, handle->Channel, __HAL_TIM_GET_AUTORELOAD(handle->htim) - initPos);
}

void Servo_Start(ServoHandle *handle)
{
  HAL_TIM_PWM_Start(handle->htim, handle->Channel);
}

void Servo_Go(ServoHandle *handle, uint16_t pos)
{
  int8_t i;
  uint16_t startPulse, currPulse, endPulse;
  uint16_t threP[3] = {10, 5, 2};
  float threD[3] = {3.0, 2.0, 1.5};

  endPulse = __HAL_TIM_GET_AUTORELOAD(handle->htim) - pos;
  startPulse = __HAL_TIM_GET_COMPARE(handle->htim, handle->Channel);
  currPulse = startPulse;
  if (currPulse < endPulse)
  {
    for (i = 0; i < 3; i++)
    {
      threP[i] = startPulse + (endPulse - startPulse) / threP[i];
    }
    for (; currPulse <= endPulse; currPulse++)
    {
      for (i = 2; i >= 0; i--)
      {
        if (currPulse <= threP[i])
        {
          Delay_us(handle->delay * threD[i]);
          break;
        }
      }
      if (i < 0)
        Delay_us(handle->delay);
      __HAL_TIM_SET_COMPARE(handle->htim, handle->Channel, currPulse);
    }
  }
  else
  {
    for (i = 0; i < 3; i++)
    {
      threP[i] = startPulse - (startPulse - endPulse) / threP[i];
    }
    for (; currPulse >= endPulse; currPulse--)
    {
      for (i = 2; i >= 0; i--)
      {
        if (currPulse >= threP[i])
        {
          Delay_us(handle->delay * threD[i]);
          break;
        }
      }
      if (i < 0)
        Delay_us(handle->delay);
      __HAL_TIM_SET_COMPARE(handle->htim, handle->Channel, currPulse);
    }
  }
}

void Servo_SetDelay(ServoHandle *handle, uint16_t delay)
{
  handle->delay = delay;
}