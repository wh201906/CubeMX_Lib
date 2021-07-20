#include "ADF4351/adf4351.h"
#include "DELAY/delay.h"
#include "UTIL/util.h"

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

#define ADF4351_R0_DEFAULT 0X002C8018
#define ADF4351_R1_DEFAULT 0X00008029
#define ADF4351_R2_DEFAULT 0X00010E42
#define ADF4351_R3_DEFAULT 0X000404B3
#define ADF4351_R4_DEFAULT 0X00EC803C
#define ADF4351_R5_DEFAULT 0X00580005

#define ADF4351_R0_INT_MASK 0x7FFF8000
#define ADF4351_R0_FRAC_MASK 0x00007FF8
#define ADF4351_R1_MOD_MASK 0X00007FF8
#define ADF4351_R2_DOUBLER 0x02000000
#define ADF4351_R2_RDIV2 0x01000000
#define ADF4351_R2_R_MASK 0x00FFC000
#define ADF4351_R4_RFDIV_MASK 0x00700000

#define ADF4351_R4_RFOUT_ON 0x00000020
#define ADF4351_R4_VCO_POWERDOWN 0x00000800
#define ADF4351_R2_CHIP_POWERDOWN 0x00000020

uint32_t ADF4351_DelayTicks;

uint32_t ADF4351_R[6];

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
	ADF4351_GPIO_Init();
	ADF4351_DelayTicks = Delay_GetSYSFreq() / 30000000.0 + 0.5; // around 33ns

	ADF4351_R[5] = ADF4351_R5_DEFAULT;
	ADF4351_R[4] = ADF4351_R4_DEFAULT;
	ADF4351_R[3] = ADF4351_R3_DEFAULT;
	ADF4351_R[2] = ADF4351_R2_DEFAULT;
	ADF4351_R[1] = ADF4351_R1_DEFAULT;
	ADF4351_R[0] = ADF4351_R0_DEFAULT;

	ADF4351_Write(ADF4351_R[5]);
	ADF4351_Write(ADF4351_R[4]);
	ADF4351_Write(ADF4351_R[3]);
	ADF4351_Write(ADF4351_R[2]);
	ADF4351_Write(ADF4351_R[1]);
	ADF4351_Write(ADF4351_R[0]);
}

void ADF4351_SetRef(ADF4351_CLKConfig *config, double freqRef)
{
	config->ref = freqRef;
}

// use isDoubled for lower external clock
// use is2Divided for 50% duty cycle of clock, which is useful in cycle slip reduction
double ADF4351_SetPFD(ADF4351_CLKConfig *config, double freqPFD, uint8_t isDoubled, uint8_t is2Divided)
{
	config->D = !!isDoubled;
	config->T = !!is2Divided;
	config->R = config->ref * (1 + config->D) / freqPFD / (1 + config->T);
	return ADF4351_GetPFD(config);
}

double ADF4351_GetPFD(ADF4351_CLKConfig *config)
{
	return (config->ref * (1 + config->D) / config->R / (1 + config->T));
}

uint8_t ADF4351_SetDiv(ADF4351_CLKConfig *config, uint8_t div)
{
	uint8_t n;
	if (div == 0)
		div = 1;
	else if (div > 64)
		div = 64;
	n = 0;
	while ((div & 0x01) == 0)
	{
		div >>= 1;
		n++;
	}
	config->Div_n = n;
	return ADF4351_GetDiv(config);
}

uint8_t ADF4351_GetDiv(ADF4351_CLKConfig *config)
{
	return (1 << config->Div_n);
}

double ADF4351_SetResolution(ADF4351_CLKConfig *config, double resolution) // resolution in MHz
{
	uint64_t numerator, denominator, factor;

	// Fraction PFD
	numerator = config->ref * (1 + config->D) * 10000000; // 0.1ppm
	denominator = config->R * (1 + config->T) * 10000000;
	factor = mygcd(numerator, denominator);
	numerator /= factor;
	denominator /= factor;

	denominator *= ADF4351_GetDiv(config);

	// Fraction resolution
	denominator *= resolution * 10000000; // 0.1ppm
	numerator *= 10000000;
	factor = mygcd(numerator, denominator);

	numerator /= factor;
	config->MOD = numerator;
	return ADF4351_GetResolution(config);
}

double ADF4351_GetResolution(ADF4351_CLKConfig *config)
{
	return (ADF4351_GetPFD(config) / ADF4351_GetDiv(config) / config->MOD);
}

double ADF4351_SetCLKConfig(ADF4351_CLKConfig *config, double freqRef, double freqPFD, uint8_t isDoubled, uint8_t is2Divided, uint8_t div, double resolution)
{
	ADF4351_SetRef(config, freqRef);
	ADF4351_SetPFD(config, freqPFD, isDoubled, is2Divided);
	ADF4351_SetDiv(config, div);
	return ADF4351_SetResolution(config, resolution);
}

void ADF4351_WriteCLKConfig(ADF4351_CLKConfig *config)
{
	ADF4351_R[4] &= ~ADF4351_R4_RFDIV_MASK;
	ADF4351_R[4] |= config->Div_n << 20;
	ADF4351_R[2] &= ~(ADF4351_R2_DOUBLER | ADF4351_R2_RDIV2 | ADF4351_R2_R_MASK);
	ADF4351_R[2] |= (config->D << 25 | config->T << 24 | config->R << 14);
	ADF4351_R[1] &= ~ADF4351_R1_MOD_MASK;
	ADF4351_R[1] |= config->MOD << 3;
	ADF4351_Write(ADF4351_R[4]);
	ADF4351_Write(ADF4351_R[2]);
	ADF4351_Write(ADF4351_R[1]);
	ADF4351_Write(ADF4351_R[0]); // Necessary for DBR/DBB
}

double ADF4351_SetFreq(ADF4351_CLKConfig *config, double freq)
{
	double resolution;
	uint32_t val;
	uint16_t INT, FRAC;
	resolution = ADF4351_GetResolution(config);
	val = freq / resolution;
	INT = val / config->MOD;
	FRAC = val % config->MOD;
	ADF4351_R[0] &= ~(ADF4351_R0_INT_MASK | ADF4351_R0_FRAC_MASK);
	ADF4351_R[0] |= (INT << 15 | FRAC << 3);
	ADF4351_Write(ADF4351_R[1]); // re-lock, when the PLL gives a wrong frequency, Write R1/R5 then R0 might help
	ADF4351_Write(ADF4351_R[0]);
	return ((INT * config->MOD + FRAC) * resolution);
}

uint8_t ADF4351_CalcDiv(double freqOut)
{
	uint8_t div;
	if (freqOut < 35.0)
		freqOut = 35.0;
	else if (freqOut > 4400.0)
		freqOut = 4400.0;

	div = 1;
	while (freqOut < 2200.0)
	{
		freqOut *= 2;
		div *= 2;
	}

	return div;
}

void ADF4351_SetOutputPower(uint32_t pwr)
{
	ADF4351_R[4] &= ~ADF4351_R4_PWR_MASK;
	ADF4351_R[4] |= pwr;
	ADF4351_Write(ADF4351_R[4]);
}