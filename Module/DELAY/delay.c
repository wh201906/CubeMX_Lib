#include "DELAY/delay.h"

static uint32_t fac_us=0;							//us延时倍乘数
			   
//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟固定为AHB时钟的1/8
//SYSCLK:系统时钟频率
void delay_init(uint16_t SYSCLK)
{
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK
	fac_us=SYSCLK;						    //不论是否使用OS,fac_us都需要使用
}								    

void delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
}
 
//延时nms
//nms:要延时的ms数
void delay_ms(uint16_t nms)
{
	HAL_Delay(nms-1);
}

