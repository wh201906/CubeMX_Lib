#include "wavegen.h"

TIM_HandleTypeDef WaveGen_TIM_Handler;
DMA_HandleTypeDef WaveGen_DMA_Handler;
DAC_HandleTypeDef WaveGen_DAC_Handler;
TIM_OC_InitTypeDef TIM_OCIniter={0};


WaveGen_WaveType WaveGen_currentWaveType;

uint16_t WaveGen_dataBuffer[WAVEGEN_BUFFER_MAX_SIZE];

void WaveGen_DACInit()
{
    DAC_ChannelConfTypeDef DAC_ChannerConf={0};
    
#ifdef STM32F407xx
    __HAL_RCC_DAC_CLK_ENABLE();
    WaveGen_DAC_Handler.Instance=DAC;
#endif
#ifdef STM32H750xx
    __HAL_RCC_DAC12_CLK_ENABLE();
    WaveGen_DAC_Handler.Instance=DAC1;
#endif
    
    HAL_DAC_Init(&WaveGen_DAC_Handler);
    
    DAC_ChannerConf.DAC_Trigger = DAC_TRIGGER_NONE;
    DAC_ChannerConf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
#ifdef STM32H750xx
    DAC_ChannerConf.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
    DAC_ChannerConf.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
    DAC_ChannerConf.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
#endif
    HAL_DAC_ConfigChannel(&WaveGen_DAC_Handler, &DAC_ChannerConf, DAC_CHANNEL_2);
}

void WaveGen_DMAInit()
{
    __HAL_RCC_DMA1_CLK_ENABLE();
#ifdef STM32F407xx
    WaveGen_DMA_Handler.Instance = DMA1_Stream1;
    WaveGen_DMA_Handler.Init.Channel = DMA_CHANNEL_3;
#endif
#ifdef STM32H750xx
    WaveGen_DMA_Handler.Instance = DMA1_Stream0;
    WaveGen_DMA_Handler.Init.Request = DMA_REQUEST_TIM2_UP;
#endif

    WaveGen_DMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;
    WaveGen_DMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
    WaveGen_DMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
    WaveGen_DMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    WaveGen_DMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    WaveGen_DMA_Handler.Init.Mode = DMA_CIRCULAR;
    WaveGen_DMA_Handler.Init.Priority = DMA_PRIORITY_LOW;
    WaveGen_DMA_Handler.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&WaveGen_DMA_Handler);
    __HAL_LINKDMA(&WaveGen_DAC_Handler,DMA_Handle2,WaveGen_DMA_Handler);

}

void WaveGen_TimerInit()
{   
    TIM_MasterConfigTypeDef TIM_MasterConf={0};
    TIM_ClockConfigTypeDef TIM_CLKSourceConf = {0};
    
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    WaveGen_TIM_Handler.Instance=TIM2;
    WaveGen_TIM_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;
    WaveGen_TIM_Handler.Init.Prescaler=12;
    WaveGen_TIM_Handler.Init.Period=1;
    WaveGen_TIM_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    WaveGen_TIM_Handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&WaveGen_TIM_Handler);
    
    TIM_CLKSourceConf.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&WaveGen_TIM_Handler, &TIM_CLKSourceConf);
    
    // not necessary for PWM
    TIM_MasterConf.MasterOutputTrigger = TIM_TRGO_UPDATE;
    TIM_MasterConf.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    HAL_TIMEx_MasterConfigSynchronization(&WaveGen_TIM_Handler,&TIM_MasterConf);
    
    

}

void WaveGen_setPWMState(uint8_t state)
{
    GPIO_InitTypeDef GPIO_Initer;
    GPIO_Initer.Pin=GPIO_PIN_5;
    GPIO_Initer.Pull=GPIO_NOPULL;
    GPIO_Initer.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
    if(state)
    {

        TIM_OCIniter.OCMode=TIM_OCMODE_PWM1;
        TIM_OCIniter.OCPolarity=TIM_OCPOLARITY_HIGH;
        TIM_OCIniter.Pulse=1; // must be higher than 0;
        TIM_OCIniter.OCFastMode = TIM_OCFAST_DISABLE;
        HAL_TIM_PWM_ConfigChannel(&WaveGen_TIM_Handler,&TIM_OCIniter,TIM_CHANNEL_1);
        HAL_TIM_OC_Start(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
        
        HAL_DAC_Stop(&WaveGen_DAC_Handler,DAC_CHANNEL_2); // necessary, and MUST be used BEFORE __HAL_RCC_DMA1_CLK_DISABLE()
        
        __HAL_RCC_DMA1_CLK_DISABLE(); // necessary, and MUST be used AFTER HAL_DAC_Stop_DMA()
#ifdef STM32F407xx
        __HAL_RCC_DAC_CLK_DISABLE(); // necessary, and MUST be used AFTER HAL_DAC_Stop_DMA()
#endif
#ifdef STM32H750xx
        __HAL_RCC_DAC12_CLK_DISABLE(); // necessary, and MUST be used AFTER HAL_DAC_Stop_DMA()
#endif
    
        GPIO_Initer.Mode=GPIO_MODE_AF_PP;
        GPIO_Initer.Alternate=GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOA,&GPIO_Initer);
        HAL_TIM_PWM_Stop(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
        HAL_TIM_PWM_Start(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
    }
    else
    {
        __HAL_RCC_DMA1_CLK_ENABLE(); // necessary, and MUST be used BEFORE HAL_DAC_Start_DMA()
#ifdef STM32F407xx
        __HAL_RCC_DAC_CLK_ENABLE(); // necessary, and MUST be used BEFORE HAL_DAC_Start_DMA()
#endif
#ifdef STM32H750xx
        __HAL_RCC_DAC12_CLK_ENABLE(); // necessary, and MUST be used BEFORE HAL_DAC_Start_DMA()
#endif        
        HAL_DAC_Start(&WaveGen_DAC_Handler,DAC_CHANNEL_2);
        __HAL_TIM_ENABLE_DMA(&WaveGen_TIM_Handler, TIM_DMA_UPDATE);
#ifdef STM32F407xx
        HAL_DMA_Start(&WaveGen_DMA_Handler,(uint32_t)WaveGen_dataBuffer,(uint32_t)(&(DAC->DHR12R2)),WAVEGEN_BUFFER_MAX_SIZE);
#endif
#ifdef STM32H750xx
        HAL_DMA_Start(&WaveGen_DMA_Handler,(uint32_t)WaveGen_dataBuffer,(uint32_t)(&(DAC1->DHR12R2)),WAVEGEN_BUFFER_MAX_SIZE); 
#endif        
        HAL_DAC_Start(&WaveGen_DAC_Handler,DAC_CHANNEL_2); // necessary , and MUST be used AFTER __HAL_RCC_DMA1_CLK_ENABLE()

        GPIO_Initer.Mode=GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA,&GPIO_Initer);
        HAL_TIM_OC_Stop(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
        //HAL_TIM_Base_Start(&WaveGen_TIM_Handler); // On my H7 board, this function doesn't work.
        __HAL_TIM_ENABLE(&WaveGen_TIM_Handler); // Works fine on my F4 and H7 board.
    }
    
}

void WaveGen_setTimerState(uint8_t state)
{
    if(state)
        HAL_TIM_PWM_Start(&WaveGen_TIM_Handler, TIM_CHANNEL_1);
    else
        HAL_TIM_Base_Stop(&WaveGen_TIM_Handler);
}

void WaveGen_setDataBuffer(WaveGen_WaveType waveType,uint16_t vpp,uint16_t len) // 0~4096 -> 0~3.3V
{
    uint16_t base = 2048;
    if (vpp > 3300)
        vpp = 3300;
    double amp = vpp * 2047.0 / 3300.0;
    uint16_t i;
    if (waveType == WAVEGEN_WAVETYPE_SINE)
    {
        for (i = 0; i < len; i++)
        {
            WaveGen_dataBuffer[i] = (uint16_t)(base + sin((double)i * 6.283185307 / len) * amp);
        }
    }
    else if (waveType == WAVEGEN_WAVETYPE_RAMP)
    {
        for (i = 0; i < len; i++)
        {
            WaveGen_dataBuffer[i] = (uint16_t)(base + ((double)i / len - 0.5) * 2 * amp);
        }
    }
    else if (waveType == WAVEGEN_WAVETYPE_SQUARE)
    {
        for (i = 0; i < len / 2; i++)
        {
            WaveGen_dataBuffer[i] = base + amp;
        }
        for (i = len / 2; i < len; i++)
        {
            WaveGen_dataBuffer[i] = base - amp;
        }
    }
    else if (waveType == WAVEGEN_WAVETYPE_TRIANGLE)
    {
        for (i = 0; i < len / 2; i++)
        {
            WaveGen_dataBuffer[i] = (uint16_t)(base + ((double)i / len - 0.25) * 4 * amp);
        }
        for (i = len / 2; i < len; i++)
        {
            WaveGen_dataBuffer[i] = (uint16_t)(base + ((double)(len - i) / len - 0.25) * 4 * amp);
        }
    }
}