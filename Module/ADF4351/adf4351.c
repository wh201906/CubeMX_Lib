/**********************************************************
                       ��������
										 
���ܣ�ADF4351���Ҳ���Ƶ�������Χ35M-4400M������0.1M��stm32f103rct6����
			��ʾ��12864
�ӿڣ�CLK-PC11 DATA-PC10 LE-PC9 CE-PC12  �����ӿ������key.h
ʱ�䣺2015/11/3
�汾��1.0
���ߣ���������
������

������������뵽�Ա��꣬�������ӽ߳�Ϊ������ ^_^
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

void ADF_Output_GPIOInit(void)
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

void WriteToADF4351(uint8_t count, uint8_t *buf)
{
	uint8_t ValueToWrite = 0;
	uint8_t i = 0;
	uint8_t j = 0;

	//	ADF_Output_GPIOInit();

	ADF4351_CE(1);
	Delay_ticks(ADF4351_DelayTicks);
	ADF4351_CLK(0);
	ADF4351_LE(0);
	Delay_ticks(ADF4351_DelayTicks);

	for (i = count; i > 0; i--)
	{
		ValueToWrite = *(buf + i - 1);
		for (j = 0; j < 8; j++)
		{
			if (0x80 == (ValueToWrite & 0x80))
			{
				ADF4351_DATA(1);
			}
			else
			{
				ADF4351_DATA(0);
			}
			Delay_ticks(ADF4351_DelayTicks);
			ADF4351_CLK(1);
			Delay_ticks(ADF4351_DelayTicks);
			ValueToWrite <<= 1;
			ADF4351_CLK(0);
		}
	}
	ADF4351_DATA(0);
	Delay_ticks(ADF4351_DelayTicks);
	ADF4351_LE(1);
	Delay_ticks(ADF4351_DelayTicks);
	ADF4351_LE(0);
}

void ADF4351Init(void)
{
	uint8_t buf[4] = {0, 0, 0, 0};

	ADF_Output_GPIOInit();
	ADF4351_DelayTicks = Delay_GetSYSFreq() / 30000000; // around 33ns

	buf[3] = 0x00;
	buf[2] = 0x58;
	buf[1] = 0x00; //write communication register 0x00580005 to control the progress
	buf[0] = 0x05; //to write Register 5 to set digital lock detector
	WriteToADF4351(4, buf);

	buf[3] = 0x00;
	buf[2] = 0xec; //(DB23=1)The signal is taken from the VCO directly;(DB22-20:4H)the RF divider is 16;(DB19-12:50H)R is 80
	buf[1] = 0x80; //(DB11=0)VCO powerd up;
	buf[0] = 0x3C; //(DB5=1)RF output is enabled;(DB4-3=3H)Output power level is 5
	WriteToADF4351(4, buf);

	buf[3] = 0x00;
	buf[2] = 0x00;
	buf[1] = 0x04; //(DB14-3:96H)clock divider value is 150.
	buf[0] = 0xB3;
	WriteToADF4351(4, buf);

	buf[3] = 0x00;
	buf[2] = 0x01;					//(DB6=1)set PD polarity is positive;(DB7=1)LDP is 6nS;
	buf[1] = 0x0E;					//(DB8=0)enable fractional-N digital lock detect;
	buf[0] = 0x42;					//(DB12-9:7H)set Icp 2.50 mA;
	WriteToADF4351(4, buf); //(DB23-14:1H)R counter is 1

	buf[3] = 0x00;
	buf[2] = 0x00;
	buf[1] = 0x80;					//(DB14-3:6H)MOD counter is 6;
	buf[0] = 0x29;					//(DB26-15:6H)PHASE word is 1,neither the phase resync
	WriteToADF4351(4, buf); //nor the spurious optimization functions are being used
													//(DB27=1)prescaler value is 8/9

	buf[3] = 0x00;
	buf[2] = 0x2c;
	buf[1] = 0x80;
	buf[0] = 0x18;					//(DB14-3:0H)FRAC value is 0;
	WriteToADF4351(4, buf); //(DB30-15:140H)INT value is 320;
}
void WriteOneRegToADF4351(uint32_t Regster)
{
	uint8_t buf[4] = {0, 0, 0, 0};
	buf[3] = (uint8_t)((Regster >> 24) & (0X000000FF));
	buf[2] = (uint8_t)((Regster >> 16) & (0X000000FF));
	buf[1] = (uint8_t)((Regster >> 8) & (0X000000FF));
	buf[0] = (uint8_t)((Regster) & (0X000000FF));
	WriteToADF4351(4, buf);
}
void ADF4351_Init_some(void)
{
	WriteOneRegToADF4351(ADF4351_R2);
	WriteOneRegToADF4351(ADF4351_R3);
	WriteOneRegToADF4351(ADF4351_R5);
}

void ADF4351WriteFreq(float Fre) //	fre��λMHz -> (xx.x) M Hz
{
	uint16_t Fre_temp, N_Mul = 1, Mul_Core = 0;
	uint16_t INT_Fre, Frac_temp, Mod_temp, i;
	uint32_t W_ADF4351_R0 = 0, W_ADF4351_R1 = 0, W_ADF4351_R4 = 0;
	float multiple;

	if (Fre < 35.0)
		Fre = 35.0;
	if (Fre > 4400.0)
		Fre = 4400.0;
	Mod_temp = 1000;
	Fre = ((float)((uint32_t)(Fre * 10))) / 10;

	Fre_temp = Fre;
	for (i = 0; i < 10; i++)
	{
		if (((Fre_temp * N_Mul) >= 2199.9) && ((Fre_temp * N_Mul) <= 4400.1))
			break;
		Mul_Core++;
		N_Mul = N_Mul * 2;
	}

	multiple = (Fre * N_Mul) / 25; //25������Ƶ�ʣ�����100M�ο������Ĵ���4��Ƶ��25M���ࡣ���û�����Ϊ80M�ο����룬�轫25��Ϊ20��10M�ο����룬�轫25��Ϊ2.5���Դ����ơ�����
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
	WriteOneRegToADF4351(ADF4351_RF_OFF);
	WriteOneRegToADF4351(W_ADF4351_R1);
	WriteOneRegToADF4351(W_ADF4351_R0);
	WriteOneRegToADF4351(W_ADF4351_R4);
	//	WriteOneRegToADF4351(ADF4351_PD_ON);
}
