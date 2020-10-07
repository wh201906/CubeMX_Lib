#include "WAVEGEN/wavegen.h"

TIM_HandleTypeDef WaveGen_TIM_Handler;
DMA_HandleTypeDef WaveGen_DMA_Handler;

WaveGen_WaveType WaveGen_currentWaveType;

uint16_t WaveGen_dataBuffer[WAVEGEN_BUFFER_SIZE];

void WaveGen_DMAInit()
{
    __HAL_RCC_DMA2_CLK_ENABLE();
}

void WaveGen_TimerInit()
{   
    TIM_MasterConfigTypeDef TIM_MasterConf={0};
    TIM_OC_InitTypeDef TIM_OCIniter={0};
    TIM_ClockConfigTypeDef TIM_CLKSourceConf = {0};
    
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    WaveGen_TIM_Handler.Instance=TIM2;
    WaveGen_TIM_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;
    WaveGen_TIM_Handler.Init.Prescaler=9;
    WaveGen_TIM_Handler.Init.Period=42;
    WaveGen_TIM_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    WaveGen_TIM_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&WaveGen_TIM_Handler);
    
    TIM_CLKSourceConf.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&WaveGen_TIM_Handler, &TIM_CLKSourceConf);
    
    // not necessary for PWM
    TIM_MasterConf.MasterOutputTrigger = TIM_TRGO_UPDATE;
    TIM_MasterConf.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    HAL_TIMEx_MasterConfigSynchronization(&WaveGen_TIM_Handler,&TIM_MasterConf);
    
    
    TIM_OCIniter.OCMode=TIM_OCMODE_PWM1;
    TIM_OCIniter.OCPolarity=TIM_OCPOLARITY_HIGH;
    TIM_OCIniter.Pulse=21; // must be higher than 0;
    TIM_OCIniter.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&WaveGen_TIM_Handler,&TIM_OCIniter,TIM_CHANNEL_1);
    HAL_TIM_OC_Start(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
    
    //HAL_TIM_Base_Start(&WaveGen_TIM_Handler);
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

void WaveGen_setDataBuffer(WaveGen_WaveType waveType,uint16_t vpp) // 0~4096 -> 0~3.3V
{
    uint16_t base = 2048;
    if (vpp > 3300)
        vpp = 3300;
    double amp = vpp * 2047.0 / 3300.0;
    uint16_t i;
    if (waveType == WAVEGEN_WAVETYPE_SINE)
    {
        for (i = 0; i < WAVEGEN_BUFFER_SIZE; i++)
        {
            WaveGen_dataBuffer[i] = (uint16_t)(base + sin((double)i / WAVEGEN_BUFFER_SIZE * 6.283185307) * amp);
        }
    }
    else if (waveType == WAVEGEN_WAVETYPE_RAMP)
    {
        for (i = 0; i < WAVEGEN_BUFFER_SIZE; i++)
        {
            WaveGen_dataBuffer[i] = (uint16_t)(base + ((double)i / WAVEGEN_BUFFER_SIZE - 0.5) * 2 * amp);
        }
    }
    else if (waveType == WAVEGEN_WAVETYPE_TRIANGLE)
    {
        for (i = 0; i < WAVEGEN_BUFFER_SIZE / 2; i++)
        {
            WaveGen_dataBuffer[i] = (uint16_t)(base + ((double)i / WAVEGEN_BUFFER_SIZE - 0.25) * 4 * amp);
        }
        for (i = WAVEGEN_BUFFER_SIZE / 2; i < WAVEGEN_BUFFER_SIZE; i++)
        {
            WaveGen_dataBuffer[i] = (uint16_t)(base + ((double)(WAVEGEN_BUFFER_SIZE - i) / WAVEGEN_BUFFER_SIZE - 0.25) * 4 * amp);
        }
    }
}