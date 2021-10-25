#include "pe4302.h"
#include "DELAY/delay.h"

uint16_t PE4302_DelayTicks = 1000;

// Pull P/S high to use serial mode

#define PE4302_LE(__STATE__) HAL_GPIO_WritePin(PE4302_LE_GPIO, PE4302_LE_PIN, (__STATE__))
#define PE4302_CLK(__STATE__) HAL_GPIO_WritePin(PE4302_CLK_GPIO, PE4302_CLK_PIN, (__STATE__))
#define PE4302_DATA(__STATE__) HAL_GPIO_WritePin(PE4302_DATA_GPIO, PE4302_DATA_PIN, (__STATE__))
#define PE4302_DELAY() Delay_ticks(PE4302_DelayTicks)

void PE4302_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	PE4302_LE_CLKEN();
	PE4302_CLK_CLKEN();
	PE4302_DATA_CLKEN();

	HAL_GPIO_WritePin(PE4302_LE_GPIO, PE4302_LE_PIN, 0);
	GPIO_InitStruct.Pin = PE4302_LE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(PE4302_LE_GPIO, &GPIO_InitStruct);

	HAL_GPIO_WritePin(PE4302_CLK_GPIO, PE4302_CLK_PIN, 0);
	GPIO_InitStruct.Pin = PE4302_CLK_PIN;
	HAL_GPIO_Init(PE4302_CLK_GPIO, &GPIO_InitStruct);

	HAL_GPIO_WritePin(PE4302_DATA_GPIO, PE4302_DATA_PIN, 0);
	GPIO_InitStruct.Pin = PE4302_DATA_PIN;
	HAL_GPIO_Init(PE4302_DATA_GPIO, &GPIO_InitStruct);

	PE4302_DelayTicks = Delay_GetSYSFreq() / 10000000.0 + 1.0; // 10M, 100ns
}

void PE4302_Set(float val) // from 0dB to 31.5dB, step 0.5dB
{
	uint8_t reg, i;
	val = (val < 0) ? 0 : val;
	val = (val > 31.5) ? 31.5 : val;
	reg = val * 2 + 0.5;

	PE4302_LE(0);
	PE4302_CLK(0);
	PE4302_DELAY();
	for (i = 0; i < 6; i++)
	{
		PE4302_DATA(!!(reg & 0x20));
		reg <<= 0x1;
		PE4302_DELAY(); // t_SDSUP
		PE4302_CLK(1);
		PE4302_DELAY(); // t_ClkH
		PE4302_CLK(0);
		PE4302_DELAY(); // t_SDHLD
	}
	PE4302_DELAY(); // t_LESUP
	PE4302_LE(1);
	PE4302_DELAY(); // t_LEPW
	PE4302_LE(0);
}
