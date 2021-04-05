#include "timer.h"

uint64_t Timer_ticks = 0;
TIM_HandleTypeDef Timer_handle;

void Timer_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  TIMER_TIM_CLKEN();
  Timer_handle.Instance = TIMER_TIM;
  Timer_handle.Init.Prescaler = 0;
  Timer_handle.Init.CounterMode = TIM_COUNTERMODE_UP;

  // For 16-bit timer, the period will saturate automatically
  Timer_handle.Init.Period = 0xFFFFFFFF;
  Timer_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  Timer_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_Base_Init(&Timer_handle);
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&Timer_handle, &sClockSourceConfig);

  // If any interrupt happens between Start() and Stop(),
  // make sure the interrupt have lower priority than this one
  // or the Period(arr) is big enough.
  HAL_NVIC_SetPriority(TIMER_IRQN, 0, 0);
  HAL_NVIC_EnableIRQ(TIMER_IRQN);
}

void Timer_Start(void)
{
  Timer_ticks = 0;
  __HAL_TIM_SET_COUNTER(&Timer_handle, 0);
  __HAL_TIM_CLEAR_IT(&Timer_handle, TIM_IT_UPDATE);
  __HAL_TIM_ENABLE_IT(&Timer_handle, TIM_IT_UPDATE);
  __HAL_TIM_ENABLE(&Timer_handle);
}

uint64_t Timer_Stop(void)
{
  uint64_t result;
  result = __HAL_TIM_GET_COUNTER(&Timer_handle);
  __HAL_TIM_DISABLE_IT(&Timer_handle, TIM_IT_UPDATE);
  __HAL_TIM_DISABLE(&Timer_handle);
  result += __HAL_TIM_GET_AUTORELOAD(&Timer_handle) * Timer_ticks;
  return result;
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
  __HAL_TIM_CLEAR_IT(&Timer_handle, TIM_IT_UPDATE);
  Timer_ticks++;
}