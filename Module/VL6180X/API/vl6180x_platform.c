#include "vl6180x_platform.h"
#include "DELAY/delay.h"
#include "I2C/softi2c2.h"

void VL6180x_PollDelay(VL6180xDev_t dev)
{
  Delay_ms(1);
}

int VL6180x_WrByte(VL6180xDev_t dev, uint16_t index, uint8_t data)
{
  return VL6180x_WrMulti(dev, index, &data, 1);
}

int VL6180x_UpdateByte(VL6180xDev_t dev, uint16_t index, uint8_t AndData, uint8_t OrData)
{
  int result = 0;
  uint8_t data;

  result = VL53L0X_RdByte(Dev, index, &data);
  if (result != 0)
    return result;

  data = (data & AndData) | OrData;
  result = VL53L0X_WrByte(Dev, index, data);
  return result;
}

int VL6180x_WrWord(VL6180xDev_t dev, uint16_t index, uint16_t data)
{
  uint8_t buffer[2] = {data >> 8, data & 0xFF};
  return VL6180x_WrMulti(dev, index, buffer, 2);
}

int VL6180x_WrDWord(VL6180xDev_t dev, uint16_t index, uint32_t data)
{
  uint8_t buffer[4] = {(data >> 24) & 0xFF, (data >> 16) & 0xFF, (data >> 8) & 0xFF, data & 0xFF};
  return VL6180x_WrMulti(dev, index, buffer, 4);
}

int VL6180x_RdByte(VL6180xDev_t dev, uint16_t index, uint8_t *data)
{
  return VL6180x_RdMulti(dev, index, data, 1);
}

int VL6180x_RdWord(VL6180xDev_t dev, uint16_t index, uint16_t *data)
{
  int result = 0;
  uint8_t buffer[2];
  result = VL6180x_RdMulti(Dev, index, buffer, 2);
  if (result != 0)
    return result;
  *data = ((uint16_t)buffer[0] << 8) + (uint16_t)buffer[1];
  return result;
}

int VL6180x_RdDWord(VL6180xDev_t dev, uint16_t index, uint32_t *data)
{
  int result = 0;
  uint8_t buffer[4];
  result = VL6180x_RdMulti(Dev, index, buffer, 4);
  if (result != 0)
    return result;
  *data = ((uint32_t)buffer[0] << 24) + ((uint32_t)buffer[1] << 16) + ((uint32_t)buffer[2] << 8) + (uint32_t)buffer[3];
  return result;
}

int VL6180x_RdMulti(VL6180xDev_t dev, uint16_t index, uint8_t *data, int nData)
{
  // VL6180xDev_t is defined as uint8_t in vl6180x_platform.h
  // the width of index is 16, so I cannot call SoftI2C2_Read()(only support 8bit reg address)
  uint32_t i;

  SoftI2C2_Start();
  if (!SoftI2C2_SendAddr(dev, SI2C_ADDR_7b, SI2C_WRITE))
    return 0;
  if (!SoftI2C2_SendByte_ACK(index >> 8, SI2C_ACK))
    return 0;
  if (!SoftI2C2_SendByte_ACK(index & 0xFF, SI2C_ACK))
    return 0;

  SoftI2C2_RepStart();
  if (!SoftI2C2_SendAddr(dev, SI2C_ADDR_7b, SI2C_READ))
    return 0;
  for (i = 0; i < nData - 1; i++)
    *(data + i) = SoftI2C2_ReadByte_ACK(SI2C_ACK);
  // The last reading should send NACK to end transfer
  *(data + i) = SoftI2C2_ReadByte_ACK(SI2C_NACK);
  SoftI2C2_Stop();

  return 1;
}

int VL6180x_WrMulti(VL6180xDev_t dev, uint16_t index, uint8_t *data, int nData)
{
  // similar to VL6180x_RdMulti()
  uint32_t i;

  SoftI2C2_Start();
  if (!SoftI2C2_SendAddr(dev, SI2C_ADDR_7b, SI2C_WRITE))
    return 0;
  if (!SoftI2C2_SendByte_ACK(index >> 8, SI2C_ACK))
    return 0;
  if (!SoftI2C2_SendByte_ACK(index & 0xFF, SI2C_ACK))
    return 0;
  for (i = 0; i < nData; i++)
    if (!SoftI2C2_SendByte_ACK(*(data + i), SI2C_ACK))
      return 0;
  SoftI2C2_Stop();

  return 1;
}