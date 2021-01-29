#include "softi2c1.h"

uint16_t delayTicks = 0;

void SoftI2C1_Init(uint32_t speed)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  SOFTI2C1_SCL_CLKEN();
  SOFTI2C1_SDA_CLKEN();

  SOFTI2C1_SCL(1);
  SOFTI2C1_SDA(1);
  GPIO_InitStruct.Pin = SOFTI2C1_SCL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SOFTI2C1_SCL_GPIO, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SOFTI2C1_SDA_PIN;
  HAL_GPIO_Init(SOFTI2C1_SDA_GPIO, &GPIO_InitStruct);

  delayTicks = Delay_GetSYSFreq() / speed / 2;
}

uint8_t SoftI2C1_SendAddr(uint16_t addr, uint8_t addrLen, uint8_t RorW)
{
  uint8_t buf;
  if (addrLen == SI2C_ADDR_7b)
  {
    buf = ((addr & 0x007F) << 1u) | RorW;
    return SoftI2C1_SendByte_ACK(buf, SI2C_ACK);
  }
  else
  {
    // 111100XX with first 2 bits
    buf = ((addr & 0x0003) << 1u) | RorW;
    buf |= 0x78;
    if (!SoftI2C1_SendByte_ACK(buf, SI2C_ACK))
      return 0;

    // the last 8 bits
    buf = addr & 0x00FF;
    return SoftI2C1_SendByte_ACK(buf, SI2C_ACK);
  }
}

uint8_t SoftI2C1_Read(uint16_t deviceAddr, uint8_t deviceAddrLen, uint8_t memAddr, uint8_t *dataBuf, uint32_t dataSize)
{
  uint32_t i;

  SoftI2C1_Start();
  if (!SoftI2C1_SendAddr(deviceAddr, deviceAddrLen, SI2C_WRITE))
    return 0;
  if (!SoftI2C1_SendByte_ACK(memAddr, SI2C_ACK))
    return 0;
  SoftI2C1_Stop();

  SoftI2C1_Start();
  if (!SoftI2C1_SendAddr(deviceAddr, deviceAddrLen, SI2C_READ))
    return 0;
  for (i = 0; i < dataSize; i++)
    *(dataBuf + i) = SoftI2C1_ReadByte_ACK(SI2C_ACK);
  SoftI2C1_Stop();

  return 1;
}
uint8_t SoftI2C1_Write(uint16_t deviceAddr, uint8_t deviceAddrLen, uint8_t memAddr, uint8_t *dataBuf, uint32_t dataSize)
{
  uint32_t i;

  SoftI2C1_Start();
  if (!SoftI2C1_SendAddr(deviceAddr, deviceAddrLen, SI2C_WRITE))
    return 0;
  if (!SoftI2C1_SendByte_ACK(memAddr, SI2C_ACK))
    return 0;
  for (i = 0; i < dataSize; i++)
    if (!SoftI2C1_SendByte_ACK(*(dataBuf + i), SI2C_ACK))
      return 0;
  SoftI2C1_Stop();

  return 1;
}

void SoftI2C1_Start(void)
{
  SOFTI2C1_SDA_OUT();
  SOFTI2C1_SCL(1);
  SOFTI2C1_SDA(1);
  Delay_ticks(delayTicks); // setup time
  SOFTI2C1_SDA(0);         // START: when CLK is high,DATA change form HIGH to LOW
  Delay_ticks(delayTicks); // hold time

  SOFTI2C1_SCL(0); // cannot be read
  Delay_ticks(delayTicks);
}

void SoftI2C1_Stop(void)
{
  SOFTI2C1_SDA_OUT();
  SOFTI2C1_SCL(1);
  SOFTI2C1_SDA(0);
  Delay_ticks(delayTicks);     // setup time
  SOFTI2C1_SDA(1);             // STOP: when CLK is high,DATA change form LOW to HIGH
  Delay_ticks(delayTicks * 2); // hold time(not necessary in most of the situations) and buff time(necessary)

  // when the transmition is stopped, the SCL should be high
}

void SoftI2C1_SendACK(uint8_t ACK) // 0:ACK 1:NACK
{
  SOFTI2C1_SCL(0); // change start
  SOFTI2C1_SDA_OUT();
  SOFTI2C1_SDA(ACK);
  Delay_ticks(delayTicks / 8); // data setup time
  SOFTI2C1_SCL(1);             // can be read
  Delay_ticks(delayTicks);     // hold
  SOFTI2C1_SCL(0);             // cannot be read
  Delay_ticks(delayTicks);     // data setup time & SCL_LOW & SMBus requirement
}

uint8_t SoftI2C1_WaitACK(void) // 0:ACK 1:NACK/No response
{
  uint16_t waitTime = 0;
  uint8_t result = 0;
  SOFTI2C1_SDA_IN();
  SOFTI2C1_SCL(1);
  Delay_ticks(delayTicks / 8); // data setup time
  result = SOFTI2C1_READSDA();
  Delay_ticks(delayTicks);
  if (result == SI2C_NACK)
    SoftI2C1_Stop();
  else
  {
    SOFTI2C1_SCL(0);
    Delay_ticks(delayTicks); // data setup time & SCL_LOW & SMBus requirement
  }
  return result;
}

// 8 SCL clock, just send a byte
void SoftI2C1_SendByte(uint8_t byte) // barely send a byte
{
  int8_t i;
  SOFTI2C1_SDA_OUT();
  SOFTI2C1_SCL(0);
  for (i = 7; i >= 0; i--)
  {
    SOFTI2C1_SDA((byte >> i) & 1u);
    Delay_ticks(delayTicks / 8); // data setup time
    SOFTI2C1_SCL(1);
    Delay_ticks(delayTicks); // data hold time
    SOFTI2C1_SCL(0);
    Delay_ticks(delayTicks); // data setup time & SCL_LOW & SMBus requirement
  }
}

// 8 SCL clock, just read a byte
uint8_t SoftI2C1_ReadByte(void) // barely read a byte
{
  uint8_t i, result = 0;
  SOFTI2C1_SDA_IN();
  for (i = 0; i < 8; i++)
  {
    SOFTI2C1_SCL(0);
    Delay_ticks(delayTicks); // data setup time & SCL_LOW & SMBus requirement
    SOFTI2C1_SCL(1);
    result <<= 1;
    Delay_ticks(delayTicks / 8); // data setup time
    result |= SOFTI2C1_READSDA();
    Delay_ticks(delayTicks); // data hold time
  }
  return result;
}

// 9 SCL clock, treat the 9th bit as ACK/NACK
// If SI2C_NACK is given, it will always return true(ignore ACK/NACK)
// If SI2C_ACK is given, it will retry SOFTI2C1_RETRYTIMES if no ACK is detected,
// and it will return whether the ACK is detected finally.
uint8_t SoftI2C1_SendByte_ACK(uint8_t byte, uint8_t handleACK) // handle ACK and retry
{
  uint8_t i;
  if (handleACK == SI2C_NACK)
  {
    SoftI2C1_SendByte(byte);
    SOFTI2C1_SDA_IN();
    SOFTI2C1_SCL(0);             // change start
    Delay_ticks(delayTicks / 8); // data setup time
    SOFTI2C1_SCL(1);             // can be read
    Delay_ticks(delayTicks);     // hold
    SOFTI2C1_SCL(0);             // cannot be read
    Delay_ticks(delayTicks);     // data setup time & SCL_LOW & SMBus requirement
    return 1;
  }
  else
  {
    for (i = 0; i < SOFTI2C1_RETRYTIMES; i++)
    {
      SoftI2C1_SendByte(byte);
      if (SoftI2C1_WaitACK() == SI2C_ACK)
        break;
    }
    return (i >= SOFTI2C1_RETRYTIMES);
  }
}

// 9 SCL clock, send ACK/NACK after data received
uint8_t SoftI2C1_ReadByte_ACK(uint8_t ACK)
{
  uint8_t result;
  result = SoftI2C1_ReadByte();
  SoftI2C1_SendACK(ACK);
  return result;
}
