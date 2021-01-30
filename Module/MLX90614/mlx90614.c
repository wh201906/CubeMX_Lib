#include "mlx90614.h"

uint8_t MLX90614_WriteROM(uint16_t deviceAddr, uint8_t romAddr, uint16_t data)
{
  uint8_t tmp[2];
  tmp[0] = data & 0x00FF;
  tmp[1] = (data >> 8u) & 0x00FF;
  return SoftI2C2_Write(deviceAddr, SI2C_ADDR_7b, romAddr & 0x1F | 0x20, tmp, 2);
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

uint8_t MLX90614_ReadRAM(uint16_t deviceAddr, uint8_t ramAddr, uint16_t *data)
{
  return MLX90614_ReadReg(deviceAddr, ramAddr & 0x1F, data);
}

uint8_t MLX90614_ReadROM(uint16_t deviceAddr, uint8_t romAddr, uint16_t *data)
{
  return MLX90614_ReadReg(deviceAddr, romAddr & 0x1F | 0x20, data);
}

void MLX90614_Init(void)
{
  SoftI2C2_Init(100000);
  Delay_ms(600);
}

float MLX90614_GetTemp(uint8_t reg)
{
  uint16_t temp;
  if (!MLX90614_ReadRAM(MLX90614_ADDR, reg, &temp) || (temp & 0x8000) == 0x8000)
    return 2000.0;
  else
    return (float)temp * 0.02 - 273.15;
}