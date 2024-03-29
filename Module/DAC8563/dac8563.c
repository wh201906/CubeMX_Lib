#include "dac8563.h"
#include "DELAY/delay.h"
#include "UTIL/mygpio.h"

uint16_t DAC8563_delayTicks;

void DAC8563_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	DAC8563_CLR_CLKEN();
	DAC8563_SYN_CLKEN();
	DAC8563_SCK_CLKEN();
	DAC8563_DIN_CLKEN();
	DAC8563_LD_CLKEN();

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	MyGPIO_Init(DAC8563_CLR_GPIO, DAC8563_CLR_PIN, 1);
	MyGPIO_Init(DAC8563_SYN_GPIO, DAC8563_SYN_PIN, 1);
	MyGPIO_Init(DAC8563_SCK_GPIO, DAC8563_SCK_PIN, 1);
	MyGPIO_Init(DAC8563_DIN_GPIO, DAC8563_DIN_PIN, 1);
	MyGPIO_Init(DAC8563_LD_GPIO, DAC8563_LD_PIN, 1);
	
	DAC8563_delayTicks = Delay_GetSYSFreq() * 0.000000010 + 1.0; // 10ns
}

void DAC8563_Write(uint8_t cmd, uint16_t data)
{
	uint8_t s = 0;
	uint32_t all;

	all = (cmd << 16) | data;
	DAC8563_SYN(1);
	Delay_ticks(DAC8563_delayTicks * 8); // SYNC HIGH time
	DAC8563_SYN(0);
	Delay_ticks(DAC8563_delayTicks / 2 + 1); // SYNC to SCLK negedge - SCLK HIGH time
	//DAC8563_SCK(0);
	for (s = 0; s < 24; s++)
	{
		DAC8563_DIN(!!(all & 0x800000));
		Delay_ticks(DAC8563_delayTicks); // SCLK HIGH time
		DAC8563_SCK(0);
		Delay_ticks(DAC8563_delayTicks); // SCLK LOW time
		DAC8563_SCK(1);
		all <<= 1;
	}
}

void DAC8563_Init(void)
{
	DAC8563_CLR(0);
	DAC8563_LD(1);
	DAC8563_IO_Init();
	DAC8563_Write(CMD_RESET_ALL_REG, DATA_RESET_ALL_REG);			//复位
	DAC8563_Write(CMD_PWR_UP_A_B, DATA_PWR_UP_A_B);						//上电DAC-A和DAC-B
	DAC8563_Write(CMD_INTERNAL_REF_EN, DATA_INTERNAL_REF_EN); // 启用内部参考和复位DAC以获得增益= 2
	DAC8563_Write(CMD_GAIN, DATA_GAIN_B2_A1);									// Set Gains
}

//=============================================
//
//		设置并更新DAC输出电压（更改模块跳线帽及设置DAC8563内部增益 可设置不同输出范围 ）
//
//	参数：data_a为A路输出，data_b为B路输出
//
//=============================================
void DAC8563_SetOutputAB(uint16_t data_a, uint16_t data_b)
{
	DAC8563_Write(CMD_SETA_UPDATEA, data_a);
	DAC8563_Write(CMD_SETB_UPDATEB, data_b);
	DAC8563_LD(0);
	Delay_ticks(DAC8563_delayTicks); // LDAC pulse duration
	DAC8563_LD(1);
}

void DAC8563_SetOutput(uint8_t isChannelA, uint16_t data)
{
  if(isChannelA)
    DAC8563_Write(CMD_SETA_UPDATEA, data);
  else
    DAC8563_Write(CMD_SETB_UPDATEB, data);
	DAC8563_LD(0);
	Delay_ticks(DAC8563_delayTicks); // LDAC pulse duration
	DAC8563_LD(1);
}