#include "dist_us.h"

#define Dist_US_TIM_Handler (htim2)

uint32_t Dist_US_counter[2]={0};
int8_t Dist_US_state=-1;
uint32_t Dist_US_distI=0;
float Dist_US_distF=0.0f;

uint32_t Dist_US_TIMPeriod=0xffff;


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
  __HAL_TIM_ENABLE_IT(&Dist_US_TIM_Handler,TIM_IT_UPDATE);
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==Dist_US_TIM_Handler.Instance && Dist_US_state==1)
    Dist_US_counter[1]+=Dist_US_TIMPeriod+1;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==Dist_US_TIM_Handler.Instance)
	{
    // has been set to 0, compatible with the PeriodElapsedCallback()
    Dist_US_counter[Dist_US_state]+=HAL_TIM_ReadCapturedValue(&Dist_US_TIM_Handler,TIM_CHANNEL_1);
    if(Dist_US_state==0)
      Dist_US_state++;
    else if(Dist_US_state==1)
    {
      Dist_US_state=-1;
      HAL_TIM_IC_Stop_IT(&Dist_US_TIM_Handler,TIM_CHANNEL_1);
      Dist_US_counter[1]-=Dist_US_counter[0];
      Dist_US_distI=Dist_US_counter[1]*17; // coffecient
      Dist_US_distF=Dist_US_distI*0.01;
    }	
	}
}
