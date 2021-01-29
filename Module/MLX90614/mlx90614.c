#include "mlx90614.h"

uint8_t MLX90614_WriteReg(uint16_t deviceAddr, uint8_t regAddr, uint16_t data)
{
  uint8_t tmp[2];
  tmp[0] = data & 0x00FF;
  tmp[1] = (data >> 8u) & 0x00FF;
  return SoftI2C2_Write(deviceAddr, SI2C_ADDR_7b, regAddr, tmp, 2);
}

uint8_t MLX90614_ReadReg(uint16_t deviceAddr, uint8_t regAddr, uint16_t *data)
{
  uint8_t tmp[2], res;
  res = SoftI2C2_Read(deviceAddr, SI2C_ADDR_7b, regAddr, tmp, 2);
  if (!res)
    return 0;
  *data = ((uint16_t)tmp[1] << 8u) | tmp[0];
  return 1;
}

void MLX90614_Init(void)
{
  SoftI2C2_Init(100000);
  Delay_ms(20);
}