#include "WAVEGEN/wavegen.h"

TIM_HandleTypeDef WaveGen_TIM_Handler;

enum WaveGen_WaveType
{
    WAVEGEN_WAVETYPE_SQUARE,
    WAVEGEN_WAVETYPE_SINE,
    WAVEGEN_WAVETYPE_RAMP,
    WAVEGEN_WAVETYPE_TRIANGLE,
};

void WaveGen_DMAInit()
{
    
}

void WaveGen_TimerInit()
{   
    TIM_MasterConfigTypeDef TIM_MasterConf={0};
    TIM_OC_InitTypeDef TIM_OCIniter={0};
    
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    WaveGen_TIM_Handler.Instance=TIM2;
    WaveGen_TIM_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;
    WaveGen_TIM_Handler.Init.Prescaler=5000;
    WaveGen_TIM_Handler.Init.Period=16700;
    WaveGen_TIM_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&WaveGen_TIM_Handler);
    
    // not necessary for PWM
    TIM_MasterConf.MasterOutputTrigger=TIM_TRGO_RESET;
    TIM_MasterConf.MasterSlaveMode=TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&WaveGen_TIM_Handler,&TIM_MasterConf);
    
    TIM_OCIniter.OCMode=TIM_OCMODE_PWM1;
    TIM_OCIniter.OCPolarity=TIM_OCPOLARITY_HIGH;
    TIM_OCIniter.Pulse=8400; // must be higher than 0;
    TIM_OCIniter.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&WaveGen_TIM_Handler,&TIM_OCIniter,TIM_CHANNEL_1);
    HAL_TIM_OC_Start(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
}

void WaveGen_setPWMState(uint8_t state)
{
    GPIO_InitTypeDef GPIO_Initer;
    GPIO_Initer.Pin=GPIO_PIN_5;
    GPIO_Initer.Pull=GPIO_NOPULL;
    GPIO_Initer.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
    if(state)
    {
        GPIO_Initer.Mode=GPIO_MODE_AF_PP;
        GPIO_Initer.Alternate=GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOA,&GPIO_Initer);
        HAL_TIM_PWM_Start(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
    }
    else
    {
        GPIO_Initer.Mode=GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA,&GPIO_Initer);
        HAL_TIM_PWM_Stop(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
    }
    
}

void WaveGen_setTimerState(uint8_t state)
{
    if(state)
        HAL_TIM_PWM_Start(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
    else
        HAL_TIM_Base_Stop(&WaveGen_TIM_Handler);
}