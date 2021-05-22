#include "mlx90614.h"

uint8_t MLX90614_addr = 0x5A;

SoftI2C_Port MLX90614_port;

uint8_t MLX90614_WriteROM(uint16_t deviceAddr, uint8_t romAddr, uint16_t data)
{
  uint8_t tmp[3];
  tmp[0] = data & 0x00FF;
  tmp[1] = (data >> 8u) & 0x00FF;

  tmp[2] = CRC8_CalcByte(0, deviceAddr << 1);
  tmp[2] = CRC8_CalcByte(tmp[2], romAddr & 0x1F | 0x20);
  tmp[2] = CRC8_Calc(tmp[2], tmp, 2);
  return SoftI2C_Write(&MLX90614_port, deviceAddr, romAddr & 0x1F | 0x20, tmp, 3);
}

uint8_t MLX90614_ReadReg(uint16_t deviceAddr, uint8_t regAddr, uint16_t *data)
{
  uint8_t tmp[3], res, crc;
  res = SoftI2C_Read(&MLX90614_port, deviceAddr, regAddr, tmp, 3);
  if (!res)
    return 0;
  crc = CRC8_CalcByte(0, deviceAddr << 1);
  crc = CRC8_CalcByte(crc, regAddr);
  crc = CRC8_CalcByte(crc, (deviceAddr << 1) | 1u);
  crc = CRC8_Calc(crc, tmp, 2);
  if (crc != tmp[2])
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

void MLX90614_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  CRC8_Init(0x07); // the polynomial of SMBus is 0x07
  SoftI2C_SetPort(&MLX90614_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&MLX90614_port, 100000, SI2C_ADDR_7b);
  Delay_ms(600);
}

float MLX90614_GetTemp(uint8_t reg)
{
  uint16_t temp;
  if (!MLX90614_ReadRAM(MLX90614_addr, reg, &temp) || (temp & 0x8000) == 0x8000)
    return 2000.0;
  else
    return (float)temp * 0.02 - 273.15;
}

void MLX90614_Sleep(uint8_t enterSleep) // 1:sleep 0:wakeup
{
  uint8_t pec = 0xE8;
  if (enterSleep)
    SoftI2C_Write(&MLX90614_port, MLX90614_addr, 0xFF, &pec, 1);
  else
  {
    SOFTI2C_SDA(&MLX90614_port, 1);
    SOFTI2C_SCL(&MLX90614_port, 0);
    Delay_ms(1);
    SOFTI2C_SDA(&MLX90614_port, 0);
    SOFTI2C_SCL(&MLX90614_port, 1);
    Delay_ms(34);
    SOFTI2C_SDA(&MLX90614_port, 1);
    Delay_ms(250);
  }
}

uint8_t MLX90614_SetI2CAddr(uint8_t addr, uint8_t changeAddrReg) // 0:just change the address in program 1:also change the address of device
{
  uint8_t result = 1;
  if (!changeAddrReg)
  {
    MLX90614_addr = addr;
    return 1;
  }
  else
  {
    result &= MLX90614_WriteROM(MLX90614_addr, MLX90614_ADDRREG, 0);
    if (!result)
    {
      result &= MLX90614_WriteROM(MLX90614_addr, MLX90614_ADDRREG, (uint16_t)MLX90614_addr);
      return 0;
    }

    Delay_ms(10);
    result &= MLX90614_WriteROM(MLX90614_addr, MLX90614_ADDRREG, (uint16_t)addr);
    if (!result)
    {
      result &= MLX90614_WriteROM(MLX90614_addr, MLX90614_ADDRREG, (uint16_t)MLX90614_addr);
      return 0;
    }
    Delay_ms(10);
    // MLX90614_addr = addr;
    // The new address will be valid after the MLX90614 is turned off then turned on.
    // Sleep then Wakeup doesn't works there.
    return 1;
  }
}