#include "ads1115.h"

SoftI2C_Port ADS1115_port;

void ADS1115_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  SoftI2C_SetPort(&ADS1115_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&ADS1115_port, 400000, SI2C_ADDR_7b);
}

uint8_t ADS1115_ReadReg(uint8_t reg, uint16_t *data)
{
  uint8_t tmp[2];
  if (!SoftI2C_Read(&ADS1115_port, ADS1115_ADDR, reg, tmp, 2))
    return 0;
  *data = ((uint16_t)tmp[0] << 8) | tmp[1];
  return 1;
}

uint8_t ADS1115_WriteReg(uint8_t reg, uint16_t data)
{
  uint8_t tmp[2] = {data >> 8, data & 0xFF};
  return SoftI2C_Write(&ADS1115_port, ADS1115_ADDR, reg, tmp, 2);
}