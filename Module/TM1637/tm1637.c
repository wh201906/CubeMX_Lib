#include "tm1637.h"

SoftI2C_Port TM1637_port;
uint16_t TM1637_currConf;

void TM1637_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  SoftI2C_SetPort(&TM1637_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&TM1637_port, 50000, SI2C_ADDR_7b);
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  GPIO_InitStruct.Pin = TM1637_port.SCL_Pin;
  HAL_GPIO_Init(TM1637_port.SCL_GPIO, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = TM1637_port.SDA_Pin;
  HAL_GPIO_Init(TM1637_port.SDA_GPIO, &GPIO_InitStruct);
}


uint8_t TM1637_Write(uint8_t *data, uint16_t len)
{
  uint16_t i;
  Delay_us(50);
  SoftI2C_Start(&TM1637_port);
  for (i = 0; i < len; i++)
    if (!SoftI2C_SendByte_ACK(&TM1637_port, data[i], SI2C_NACK))
      return 0;
  SoftI2C_Stop(&TM1637_port);
  return 1;
}

void TM1637_Test()
{
  uint8_t buf[12] = {0x40, 0xC0, 0x5A, 0x5A, 0x5A, 0x5A, 0x8f};
  TM1637_Write(buf, 1);
  TM1637_Write(buf+1, 2);
  TM1637_Write(buf+3, 1);
}