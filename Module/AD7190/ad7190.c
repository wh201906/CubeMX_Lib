#include "ad7190.h"

#define AD7190_CS(__ST__) (HAL_GPIO_WritePin(AD7190_CS_GPIO, AD7190_CS_PIN, (__ST__)))
#define AD7190_SCK(__ST__) (HAL_GPIO_WritePin(AD7190_SCK_GPIO, AD7190_SCK_PIN, (__ST__)))
#define AD7190_DIN(__ST__) (HAL_GPIO_WritePin(AD7190_DIN_GPIO, AD7190_DIN_PIN, (__ST__)))
#define AD7190_DOUT() (HAL_GPIO_ReadPin(AD7190_DOUT_GPIO, AD7190_DOUT_PIN))
#define AD7190_Delay() Delay_us(1)

uint8_t AD7190_GetID(void)
{
  AD7190_Write(0x60, 8);
  return AD7190_Read(8);
}

uint32_t AD7190_GetConf(void)
{
  AD7190_Write(0x50, 8);
  return AD7190_Read(24);
}

uint8_t AD7190_SetConf(uint32_t conf)
{
  AD7190_Write(0x10, 8);
  AD7190_Write(conf, 32);
  return (AD7190_GetConf() == conf);
}

void AD7190_Reset(void)
{
  uint8_t i;
  AD7190_DIN(1);
  AD7190_SCK(1);
  AD7190_Delay();
  AD7190_CS(1);
  AD7190_Delay();
  AD7190_CS(0);
  AD7190_Delay();
  for (i = 0; i < 42; i++) // at least 40 clk
  {
    AD7190_SCK(0);
    AD7190_Delay();
    AD7190_SCK(1);
    AD7190_Delay();
  }
  AD7190_CS(1);
  Delay_us(500);
}

uint8_t AD7190_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  AD7190_CS_CLKEN();
  AD7190_SCK_CLKEN();
  AD7190_DIN_CLKEN();
  AD7190_DOUT_CLKEN();

  AD7190_CS(1);
  GPIO_InitStruct.Pin = AD7190_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(AD7190_CS_GPIO, &GPIO_InitStruct);

  AD7190_SCK(1);
  GPIO_InitStruct.Pin = AD7190_SCK_PIN;
  HAL_GPIO_Init(AD7190_SCK_GPIO, &GPIO_InitStruct);

  AD7190_DIN(1);
  GPIO_InitStruct.Pin = AD7190_DIN_PIN;
  HAL_GPIO_Init(AD7190_DIN_GPIO, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = AD7190_DOUT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(AD7190_DOUT_GPIO, &GPIO_InitStruct);

  AD7190_Reset();
  return AD7190_GetID();
}

uint32_t AD7190_Read(uint8_t bitLen)
{
  uint32_t result = 0;
  AD7190_CS(0);
  AD7190_Delay();
  while (bitLen--)
  {
    AD7190_SCK(0);
    AD7190_Delay();
    result <<= 1;
    result |= AD7190_DOUT(); // sample before LSB changed
    AD7190_SCK(1); // the chip will not hold the LSB after the posedge 
    AD7190_Delay();
  }
  AD7190_CS(1);
  return result;
}

uint8_t AD7190_Write(uint32_t data, uint8_t bitLen)
{
  AD7190_CS(0);
  AD7190_Delay();
  data <<= 32 - bitLen; 
  while (bitLen--)
  {
    AD7190_SCK(0);
    AD7190_DIN(!!(data & 0x80000000));
    AD7190_Delay();
    AD7190_SCK(1);
    AD7190_Delay();
    data <<= 1;
  }
  AD7190_CS(1);
  return 1;
}