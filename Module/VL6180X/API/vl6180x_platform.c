#include "vl6180x_platform.h"
#include "DELAY/delay.h"

SoftI2C_Port VL6180X_port;

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

  result = VL6180x_RdByte(dev, index, &data);
  if (result != 0)
    return result;

  data = (data & AndData) | OrData;
  result = VL6180x_WrByte(dev, index, data);
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
  result = VL6180x_RdMulti(dev, index, buffer, 2);
  if (result != 0)
    return result;
  *data = ((uint16_t)buffer[0] << 8) + (uint16_t)buffer[1];
  return result;
}

int VL6180x_RdDWord(VL6180xDev_t dev, uint16_t index, uint32_t *data)
{
  int result = 0;
  uint8_t buffer[4];
  result = VL6180x_RdMulti(dev, index, buffer, 4);
  if (result != 0)
    return result;
  *data = ((uint32_t)buffer[0] << 24) + ((uint32_t)buffer[1] << 16) + ((uint32_t)buffer[2] << 8) + (uint32_t)buffer[3];
  return result;
}

int VL6180x_RdMulti(VL6180xDev_t dev, uint16_t index, uint8_t *data, int nData)
{
  return (!SoftI2C_16Read(&VL6180X_port, dev, index, data, nData));
}

int VL6180x_WrMulti(VL6180xDev_t dev, uint16_t index, uint8_t *data, int nData)
{
  return (!SoftI2C_16Write(&VL6180X_port, dev, index, data, nData));
}