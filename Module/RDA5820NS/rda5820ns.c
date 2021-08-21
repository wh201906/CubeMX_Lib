#include "rda5820ns.h"

SoftI2C_Port RDA5820_port;

void RDA5820_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  SoftI2C_SetPort(&RDA5820_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&RDA5820_port, 400000, SI2C_ADDR_7b);
}

uint8_t RDA5820_ReadReg(uint8_t reg, uint16_t *data)
{
  uint8_t tmp[2];
  if (!SoftI2C_Read(&RDA5820_port, RDA5820_ADDR, reg, tmp, 2))
    return 0;
  *data = ((uint16_t)tmp[0] << 8) | tmp[1];
  return 1;
}

uint8_t RDA5820_WriteReg(uint8_t reg, uint16_t data)
{
  uint8_t tmp[2] = {data >> 8, data & 0xFF};
  return SoftI2C_Write(&RDA5820_port, RDA5820_ADDR, reg, tmp, 2);
}

uint16_t RDA5820_ReadID(void)
{
  uint16_t tmp;
  RDA5820_ReadReg(0x00, &tmp);
  return tmp;
}