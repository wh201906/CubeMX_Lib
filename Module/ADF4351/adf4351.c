/**********************************************************
                       康威电子
										 
功能：ADF4351正弦波点频输出，范围35M-4400M，步进0.1M，stm32f103rct6控制
			显示：12864
接口：CLK-PC11 DATA-PC10 LE-PC9 CE-PC12  按键接口请参照key.h
时间：2015/11/3
版本：1.0
作者：康威电子
其他：

更多电子需求，请到淘宝店，康威电子竭诚为您服务 ^_^
https://shop110336474.taobao.com/?spm=a230r.7195193.1997079397.2.Ic3MRJ

**********************************************************/
#include "ADF4351/adf4351.h"
#include "DELAY/delay.h"

#define ADF4351_CE_PIN GPIO_PIN_10 // reversed CS
#define ADF4351_LE_PIN GPIO_PIN_11 // make postive pulse to update register
#define ADF4351_DATA_PIN GPIO_PIN_12
#define ADF4351_CLK_PIN GPIO_PIN_13

#define ADF4351_CE_GPIO GPIOB
#define ADF4351_LE_GPIO GPIOB
#define ADF4351_CLK_GPIO GPIOB
#define ADF4351_DATA_GPIO GPIOB

#define ADF4351_CE_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADF4351_LE_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADF4351_DATA_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADF4351_CLK_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()

#define ADF4351_CE(__STATE__) HAL_GPIO_WritePin(ADF4351_CE_GPIO, ADF4351_CE_PIN, (__STATE__))
#define ADF4351_LE(__STATE__) HAL_GPIO_WritePin(ADF4351_LE_GPIO, ADF4351_LE_PIN, (__STATE__))
#define ADF4351_DATA(__STATE__) HAL_GPIO_WritePin(ADF4351_DATA_GPIO, ADF4351_DATA_PIN, (__STATE__))
#define ADF4351_CLK(__STATE__) HAL_GPIO_WritePin(ADF4351_CLK_GPIO, ADF4351_CLK_PIN, (__STATE__))

//#define
#define ADF4351_R0 ((uint32_t)0X2C8018)
#define ADF4351_R1 ((uint32_t)0X8029)
#define ADF4351_R2 ((uint32_t)0X10E42)
#define ADF4351_R3 ((uint32_t)0X4B3)
#define ADF4351_R4 ((uint32_t)0XEC803C)
#define ADF4351_R5 ((uint32_t)0X580005)

#define ADF4351_R1_Base ((uint32_t)0X8001)
#define ADF4351_R4_Base ((uint32_t)0X8C803C)
#define ADF4351_R4_ON ((uint32_t)0X8C803C)
#define ADF4351_R4_OFF ((uint32_t)0X8C883C)

//#define ADF4351_RF_OFF	((uint32_t)0XEC801C)

#define ADF4351_PD_ON ((uint32_t)0X10E42)
#define ADF4351_PD_OFF ((uint32_t)0X10E02)

uint32_t ADF4351_DelayTicks;

void ADF4351_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	ADF4351_CE_CLKEN();
	ADF4351_LE_CLKEN();
	ADF4351_CLK_CLKEN();
	ADF4351_DATA_CLKEN();

	HAL_GPIO_WritePin(ADF4351_CE_GPIO, ADF4351_CE_PIN, 1);
	GPIO_InitStruct.Pin = ADF4351_CE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(ADF4351_CE_GPIO, &GPIO_InitStruct);

	HAL_GPIO_WritePin(ADF4351_LE_GPIO, ADF4351_LE_PIN, 0);
	GPIO_InitStruct.Pin = ADF4351_LE_PIN;
	HAL_GPIO_Init(ADF4351_LE_GPIO, &GPIO_InitStruct);

	// SPI, sample on posedge, clock is low when free
	HAL_GPIO_WritePin(ADF4351_CLK_GPIO, ADF4351_CLK_PIN, 0);
	GPIO_InitStruct.Pin = ADF4351_CLK_PIN;
	HAL_GPIO_Init(ADF4351_CLK_GPIO, &GPIO_InitStruct);

	HAL_GPIO_WritePin(ADF4351_DATA_GPIO, ADF4351_DATA_PIN, 0);
	GPIO_InitStruct.Pin = ADF4351_DATA_PIN;
	HAL_GPIO_Init(ADF4351_DATA_GPIO, &GPIO_InitStruct);
}

void ADF4351_Write(uint32_t val)
{
	uint8_t i;
	uint32_t ValueToWrite = 0;

	ADF4351_CE(1);
	Delay_ticks(ADF4351_DelayTicks);
	ADF4351_CLK(0);
	ADF4351_LE(0);
	Delay_ticks(ADF4351_DelayTicks);

	for (i = 0; i < 32; i++)
	{
		ADF4351_DATA(!!((val << i) & 0x80000000));
		Delay_ticks(ADF4351_DelayTicks);
		ADF4351_CLK(1);
		Delay_ticks(ADF4351_DelayTicks);
		ADF4351_CLK(0);
	}

	ADF4351_DATA(0);
	Delay_ticks(ADF4351_DelayTicks);
	ADF4351_LE(1);
	Delay_ticks(ADF4351_DelayTicks);
	ADF4351_LE(0);
}

void ADF4351_Init(void)
{
	uint8_t buf[4] = {0, 0, 0, 0};

	ADF4351_GPIO_Init();
	ADF4351_DelayTicks = Delay_GetSYSFreq() / 30000000; // around 33ns

	ADF4351_Write(0x00580005); // reg5
	ADF4351_Write(0x00EC803C); // reg4
	ADF4351_Write(0x000004B3); // reg3
	ADF4351_Write(0x00010E42); // reg2
	ADF4351_Write(0x00008029); // reg1
	ADF4351_Write(0x002C8018); // reg0
}

void ADF4351_Reg_Init(void)
{
	ADF4351_Write(ADF4351_R2);
	ADF4351_Write(ADF4351_R3);
	ADF4351_Write(ADF4351_R5);
}

void ADF4351_SetFreq(float freq) //	fre单位MHz -> (xx.x) M Hz
{
	uint16_t Fre_temp, N_Mul = 1, Mul_Core = 0;
	uint16_t INT_Fre, Frac_temp, Mod_temp, i;
	uint32_t W_ADF4351_R0 = 0, W_ADF4351_R1 = 0, W_ADF4351_R4 = 0;
	float multiple;

	if (freq < 35.0)
		freq = 35.0;
	if (freq > 4400.0)
		freq = 4400.0;
	Mod_temp = 1000;
	freq = ((float)((uint32_t)(freq * 10))) / 10;

	Fre_temp = freq;
	for (i = 0; i < 10; i++)
	{
		if (((Fre_temp * N_Mul) >= 2199.9) && ((Fre_temp * N_Mul) <= 4400.1))
			break;
		Mul_Core++;
		N_Mul = N_Mul * 2;
	}

	multiple = (freq * N_Mul) / 25; //25：鉴相频率，板载100M参考，经寄存器4分频得25M鉴相。若用户更改为80M参考输入，需将25改为20；10M参考输入，需将25改为2.5，以此类推。。。
	INT_Fre = (uint16_t)multiple;
	Frac_temp = ((uint32_t)(multiple * 1000)) % 1000;
	while (((Frac_temp % 5) == 0) && ((Mod_temp % 5) == 0))
	{
		Frac_temp = Frac_temp / 5;
		Mod_temp = Mod_temp / 5;
	}
	while (((Frac_temp % 2) == 0) && ((Mod_temp % 2) == 0))
	{
		Frac_temp = Frac_temp / 2;
		Mod_temp = Mod_temp / 2;
	}

	Mul_Core %= 7;
	W_ADF4351_R0 = (INT_Fre << 15) + (Frac_temp << 3);
	W_ADF4351_R1 = ADF4351_R1_Base + (Mod_temp << 3);
	W_ADF4351_R4 = ADF4351_R4_ON + (Mul_Core << 20);

	//	WriteOneRegToADF4351(ADF4351_PD_OFF); //ADF4351_RF_OFF
	//	WriteOneRegToADF4351((uint32_t)(ADF4351_R4_OFF + (6<<20)));
	ADF4351_Write(ADF4351_RF_OFF);
	ADF4351_Write(W_ADF4351_R1);
	ADF4351_Write(W_ADF4351_R0);
	ADF4351_Write(W_ADF4351_R4);
	//	WriteOneRegToADF4351(ADF4351_PD_ON);
}
