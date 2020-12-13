#include "dist_us.h"

TIM_HandleTypeDef Dist_US_TIM_Handler;

uint32_t Dist_US_counter[2]={0};
int8_t Dist_US_state=-1;
uint32_t Dist_US_distI=0;
float Dist_US_distF=0.0f;

uint32_t Dist_US_TIMPeriod=0xffff;

void Dist_US_Init(uint8_t SYSCLK) // 1us, coefficient=0.17
{
  
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};
  
  //Timer
  __HAL_RCC_TIM2_CLK_ENABLE();
  
  Dist_US_TIM_Handler.Instance = TIM2;
  Dist_US_TIM_Handler.Init.Prescaler = 168;
  Dist_US_TIM_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
  Dist_US_TIM_Handler.Init.Period = Dist_US_TIMPeriod;
  Dist_US_TIM_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  Dist_US_TIM_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  HAL_TIM_Base_Init(&Dist_US_TIM_Handler);
  
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&Dist_US_TIM_Handler, &sClockSourceConfig);
  
  HAL_TIM_IC_Init(&Dist_US_TIM_Handler);
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&Dist_US_TIM_Handler, &sMasterConfig);
  
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  HAL_TIM_IC_ConfigChannel(&Dist_US_TIM_Handler, &sConfigIC, TIM_CHANNEL_1);
  
  // GPIO
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  //Interrupt
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void Dist_US_Start(void)
{
  uint8_t i;
  Dist_US_counter[0]=0;
  Dist_US_counter[1]=0;
  
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
  Delay_us(40); // reset
  
  //trig
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
  Delay_us(15);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
  
  //capture echo
  HAL_TIM_IC_Start_IT(&Dist_US_TIM_Handler,TIM_CHANNEL_1);
  Dist_US_state=0;
  for(i=0;i<85;i++)
  {
    Delay_ms(1);
    if(Dist_US_state==-1)
      break;
  }
}

uint32_t Dist_US_GetDistI(void)
{
  Dist_US_Start();
  return Dist_US_distI;
}
float Dist_US_GetDistF(void)
{
  Dist_US_Start();
  return Dist_US_distF;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==Dist_US_TIM_Handler.Instance)
	{
    Dist_US_counter[Dist_US_state]=HAL_TIM_ReadCapturedValue(&Dist_US_TIM_Handler,TIM_CHANNEL_1);
    if(Dist_US_state==0)
      Dist_US_state++;
    else if(Dist_US_state==1)
    {
      Dist_US_state=-1;
      HAL_TIM_IC_Stop_IT(&Dist_US_TIM_Handler,TIM_CHANNEL_1);
      Dist_US_counter[1]-=Dist_US_counter[0];
      Dist_US_distI=Dist_US_counter[1]*17;
      Dist_US_distF=Dist_US_distI*0.01;
    }	
	}
}
