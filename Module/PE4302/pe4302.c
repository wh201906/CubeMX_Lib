/*
*Author:Iko Pang
*Date  :2019-1-20
*HDU-RF 
*/
#include "PE4302.h"
#include "usart.h"
#include "sys.h"

void PE4302_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	 //使能PA端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化									 
}

void PE4302_Set(float val)
{
	unsigned char i;
	unsigned char bit[] = {0x10,0x8,0x4,0x2,0x1,0};
//	char test[6]={0};
	unsigned char Int = (unsigned char)val;

	if(val - 1.0*((int)val) >= 0.5)
	{
		if(val - 1.0*((int)val) <= 0.75){
			bit[5] = 1;
		}
		else{
			Int++;
			bit[5] = 0;
		}
	}
	
	PE4302_LE=0;
	delay_us(10);
	for(i = 0; i < 5; i++)
	{
		if(Int&bit[i])
		{
			PE4302_DATA = 1;
//			test[i]=1;

		}else{
			PE4302_DATA = 0;
//			test[i]=0;
		}
		delay_us(1);
		PE4302_CLK = 1;
		delay_us(5);
		PE4302_CLK = 0;
		delay_us(1);
	}
	if(bit[5])
	{
		PE4302_DATA = 1;
//		test[5] = 1;
	}else{
		PE4302_DATA = 0;
//		test[5] = 0;
	}
	delay_us(1);
	PE4302_CLK = 1;
	delay_us(5);
	PE4302_CLK = 0;
	delay_us(1);
	PE4302_LE = 1;
	delay_us(1);
	PE4302_LE=0;
	delay_us(1);
}

void PE4302b_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	 //使能PA端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化									 
}

void PE4302b_Set(float val)
{
	unsigned char i;
	unsigned char bit[] = {0x10,0x8,0x4,0x2,0x1,0};
//	char test[6]={0};
	unsigned char Int = (unsigned char)val;

	if(val - 1.0*((int)val) >= 0.25)
	{
		if(val - 1.0*((int)val) <= 0.75){
			bit[5] = 1;
		}
		else{
			Int++;
			bit[5] = 0;
		}
	}
	
	PE4302b_LE=0;
	delay_us(10);
	for(i = 0; i < 5; i++)
	{
		if(Int&bit[i])
		{
			PE4302b_DATA = 1;
//			test[i]=1;

		}else{
			PE4302b_DATA = 0;
//			test[i]=0;
		}
		delay_us(1);
		PE4302b_CLK = 1;
		delay_us(5);
		PE4302b_CLK = 0;
		delay_us(1);
	}
	if(bit[5])
	{
		PE4302b_DATA = 1;
//		test[5] = 1;
	}else{
		PE4302b_DATA = 0;
//		test[5] = 0;
	}
	delay_us(1);
	PE4302b_CLK = 1;
	delay_us(5);
	PE4302b_CLK = 0;
	delay_us(1);
	PE4302b_LE = 1;
	delay_us(1);
	PE4302b_LE=0;
	delay_us(1);
}

void PE4302_DoubleSet(float val){
	u8 temp = (u8)val;
	float vala = (float)(temp/2);
	float valb = val-vala;
	
	PE4302_Set(vala);
	PE4302b_Set(valb);
}
