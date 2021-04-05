#include "timer.h"

uint64_t overflowedTicks = 0;
TIM_HandleTypeDef Timer_handle;

void Timer_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  __HAL_RCC_TIM5_CLK_ENABLE();
  Timer_handle.Instance = TIM5;
  Timer_handle.Init.Prescaler = 0;
  Timer_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  Timer_handle.Init.Period = 0xFF;
  Timer_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  Timer_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_Base_Init(&Timer_handle);
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&Timer_handle, &sClockSourceConfig);

  // If any interrupt happens between Start() and Stop(),
  // make sure the interrupt have lower priority than this one
  // or the Period(arr) is big enough.
  HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM5_IRQn);
}

void Timer_Start(void)
{
  overflowedTicks = 0;
  __HAL_TIM_SET_COUNTER(&Timer_handle, 0);
  __HAL_TIM_ENABLE_IT(&Timer_handle, TIM_IT_UPDATE);
  __HAL_TIM_ENABLE(&Timer_handle);
}

uint64_t Timer_Stop(void)
{
  uint64_t result;
  result = __HAL_TIM_GET_COUNTER(&Timer_handle);
  __HAL_TIM_DISABLE_IT(&Timer_handle, TIM_IT_UPDATE);
  __HAL_TIM_DISABLE(&Timer_handle);
  result += __HAL_TIM_GET_AUTORELOAD(&Timer_handle) * overflowedTicks;
  return result;
}

void TIM5_IRQHandler(void)
{
  __HAL_TIM_CLEAR_IT(&Timer_handle, TIM_IT_UPDATE);
  overflowedTicks++;
}