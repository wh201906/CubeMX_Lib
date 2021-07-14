#include "softi2c.h"

void SoftI2C_SetPort(SoftI2C_Port *port, GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  port->SCL_GPIO = SCL_GPIO;
  port->SDA_GPIO = SDA_GPIO;
  port->SCL_PinID = SCL_PinID;
  port->SDA_PinID = SDA_PinID;
  port->SCL_Pin = ((uint16_t)1u << SCL_PinID);
  port->SDA_Pin = ((uint16_t)1u << SDA_PinID);
}

void SoftI2C_Init(SoftI2C_Port *port, uint32_t speed, uint8_t addrLen)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

#if defined(__HAL_RCC_GPIOA_CLK_ENABLE) // GPIOA exists
  if (port->SCL_GPIO == GPIOA || port->SDA_GPIO == GPIOA)
    __HAL_RCC_GPIOA_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOB_CLK_ENABLE) // GPIOB exists
  if (port->SCL_GPIO == GPIOB || port->SDA_GPIO == GPIOB)
    __HAL_RCC_GPIOB_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOC_CLK_ENABLE) // GPIOC exists
  if (port->SCL_GPIO == GPIOC || port->SDA_GPIO == GPIOC)
    __HAL_RCC_GPIOC_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOD_CLK_ENABLE) // GPIOD exists
  if (port->SCL_GPIO == GPIOD || port->SDA_GPIO == GPIOD)
    __HAL_RCC_GPIOD_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOE_CLK_ENABLE) // GPIOE exists
  if (port->SCL_GPIO == GPIOE || port->SDA_GPIO == GPIOE)
    __HAL_RCC_GPIOE_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOF_CLK_ENABLE) // GPIOF exists
  if (port->SCL_GPIO == GPIOF || port->SDA_GPIO == GPIOF)
    __HAL_RCC_GPIOF_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOG_CLK_ENABLE) // GPIOG exists
  if (port->SCL_GPIO == GPIOG || port->SDA_GPIO == GPIOG)
    __HAL_RCC_GPIOG_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOH_CLK_ENABLE) // GPIOH exists
  if (port->SCL_GPIO == GPIOH || port->SDA_GPIO == GPIOH)
    __HAL_RCC_GPIOH_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOI_CLK_ENABLE) // GPIOI exists
  if (port->SCL_GPIO == GPIOI || port->SDA_GPIO == GPIOI)
    __HAL_RCC_GPIOI_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOJ_CLK_ENABLE) // GPIOJ exists
  if (port->SCL_GPIO == GPIOJ || port->SDA_GPIO == GPIOJ)
    __HAL_RCC_GPIOJ_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOK_CLK_ENABLE) // GPIOK exists(168pin max)
  if (port->SCL_GPIO == GPIOK || port->SDA_GPIO == GPIOK)
    __HAL_RCC_GPIOK_CLK_ENABLE();
#endif

  SOFTI2C_SCL(port, 1);
  SOFTI2C_SDA(port, 1);

  GPIO_InitStruct.Pin = port->SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(port->SCL_GPIO, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = port->SDA_Pin;
  HAL_GPIO_Init(port->SDA_GPIO, &GPIO_InitStruct);

  port->delayTicks = Delay_GetSYSFreq() / speed / 2;
  port->halfTicks = (port->delayTicks >> 1) + 1;
  port->addrLen = addrLen;
}

uint8_t SoftI2C_SendAddr(SoftI2C_Port *port, uint16_t addr, uint8_t RorW)
{
  uint8_t buf;
  if (port->addrLen == SI2C_ADDR_7b)
  {
    buf = ((addr & 0x007F) << 1u) | RorW;
    return SoftI2C_SendByte_ACK(port, buf, SI2C_ACK);
  }
  else
  {
    // 111100XX with first 2 bits
    buf = ((addr & 0x0003) << 1u) | RorW;
    buf |= 0x78;
    if (!SoftI2C_SendByte_ACK(port, buf, SI2C_ACK))
      return 0;

    // the last 8 bits
    buf = addr & 0x00FF;
    return SoftI2C_SendByte_ACK(port, buf, SI2C_ACK);
  }
}

uint8_t SoftI2C_Read(SoftI2C_Port *port, uint16_t deviceAddr, uint8_t memAddr, uint8_t *dataBuf, uint32_t dataSize)
{
  uint32_t i;

  SoftI2C_Start(port);
  if (!SoftI2C_SendAddr(port, deviceAddr, SI2C_WRITE))
    return 0;
  if (!SoftI2C_SendByte_ACK(port, memAddr, SI2C_ACK))
    return 0;
  // SoftI2C1_Stop(); // A STOP and START signal is required on some devices.
  // SoftI2C1_Start();

  SoftI2C_RepStart(port);
  if (!SoftI2C_SendAddr(port, deviceAddr, SI2C_READ))
    return 0;
  for (i = 0; i < dataSize - 1; i++)
    *(dataBuf + i) = SoftI2C_ReadByte_ACK(port, SI2C_ACK);
  // The last reading should send NACK to end transfer
  *(dataBuf + i) = SoftI2C_ReadByte_ACK(port, SI2C_NACK);
  SoftI2C_Stop(port);

  return 1;
}

uint8_t SoftI2C_Write(SoftI2C_Port *port, uint16_t deviceAddr, uint8_t memAddr, uint8_t *dataBuf, uint32_t dataSize)
{
  uint32_t i;

  SoftI2C_Start(port);
  if (!SoftI2C_SendAddr(port, deviceAddr, SI2C_WRITE))
    return 0;
  if (!SoftI2C_SendByte_ACK(port, memAddr, SI2C_ACK))
    return 0;
  for (i = 0; i < dataSize; i++)
    if (!SoftI2C_SendByte_ACK(port, *(dataBuf + i), SI2C_ACK))
      return 0;
  SoftI2C_Stop(port);

  return 1;
}

uint8_t SoftI2C_16Read(SoftI2C_Port *port, uint16_t deviceAddr, uint16_t memAddr, uint8_t *dataBuf, uint32_t dataSize)
{
  uint32_t i;

  SoftI2C_Start(port);
  if (!SoftI2C_SendAddr(port, deviceAddr, SI2C_WRITE))
    return 0;
  if (!SoftI2C_SendByte_ACK(port, memAddr >> 8, SI2C_ACK))
    return 0;
  if (!SoftI2C_SendByte_ACK(port, memAddr & 0xFF, SI2C_ACK))
    return 0;
  // SoftI2C1_Stop(); // A STOP and START signal is required on some devices.
  // SoftI2C1_Start();

  SoftI2C_RepStart(port);
  if (!SoftI2C_SendAddr(port, deviceAddr, SI2C_READ))
    return 0;
  for (i = 0; i < dataSize - 1; i++)
    *(dataBuf + i) = SoftI2C_ReadByte_ACK(port, SI2C_ACK);
  // The last reading should send NACK to end transfer
  *(dataBuf + i) = SoftI2C_ReadByte_ACK(port, SI2C_NACK);
  SoftI2C_Stop(port);

  return 1;
}

uint8_t SoftI2C_16Write(SoftI2C_Port *port, uint16_t deviceAddr, uint16_t memAddr, uint8_t *dataBuf, uint32_t dataSize)
{
  uint32_t i;

  SoftI2C_Start(port);
  if (!SoftI2C_SendAddr(port, deviceAddr, SI2C_WRITE))
    return 0;
  if (!SoftI2C_SendByte_ACK(port, memAddr >> 8, SI2C_ACK))
    return 0;
  if (!SoftI2C_SendByte_ACK(port, memAddr & 0xFF, SI2C_ACK))
    return 0;
  for (i = 0; i < dataSize; i++)
    if (!SoftI2C_SendByte_ACK(port, *(dataBuf + i), SI2C_ACK))
      return 0;
  SoftI2C_Stop(port);

  return 1;
}

void SoftI2C_Start(SoftI2C_Port *port)
{
  SOFTI2C_SCL(port, 1);
  SOFTI2C_SDA(port, 1);
  Delay_ticks(port->delayTicks); // setup time
  SOFTI2C_SDA(port, 0);          // START: when CLK is high,DATA change form HIGH to LOW
  Delay_ticks(port->delayTicks); // hold time

  SOFTI2C_SCL(port, 0); // cannot be read
  Delay_ticks(port->delayTicks);
}

void SoftI2C_RepStart(SoftI2C_Port *port)
{
  SOFTI2C_SCL(port, 0);
  Delay_ticks(port->halfTicks);
  SOFTI2C_SDA(port, 1);
  Delay_ticks(port->halfTicks);
  SoftI2C_Start(port);
}

void SoftI2C_Stop(SoftI2C_Port *port)
{
  SOFTI2C_SCL(port, 0);
  SOFTI2C_SDA(port, 0);
  Delay_ticks(port->halfTicks);
  SOFTI2C_SCL(port, 1);
  Delay_ticks(port->delayTicks);     // setup time
  SOFTI2C_SDA(port, 1);              // STOP: when CLK is high,DATA change form LOW to HIGH
  Delay_ticks(port->delayTicks * 2); // hold time(not necessary in most of the situations) and buff time(necessary)
  // when the transmition is stopped, the SCL should be high
}

void SoftI2C_SendACK(SoftI2C_Port *port, uint8_t ACK) // 0:ACK 1:NACK
{
  SOFTI2C_SCL(port, 0); // change start
  Delay_ticks(port->halfTicks);
  SOFTI2C_SDA(port, ACK);
  Delay_ticks(port->halfTicks);  // data setup time
  SOFTI2C_SCL(port, 1);          // can be read
  Delay_ticks(port->delayTicks); // hold
  SOFTI2C_SCL(port, 0);          // cannot be read
  Delay_ticks(port->halfTicks);  // data setup time & SCL_LOW & SMBus requirement
}

uint8_t SoftI2C_WaitACK(SoftI2C_Port *port) // 0:ACK 1:NACK/No response
{
  uint16_t waitTime = 0;
  uint8_t result = 0;
  SOFTI2C_SDA_IN(port);
  SOFTI2C_SCL(port, 0);
  Delay_ticks(port->halfTicks);
  SOFTI2C_SCL(port, 1);
  Delay_ticks(port->halfTicks); // data setup time
  result = SOFTI2C_READSDA(port);
  Delay_ticks(port->halfTicks);
  SOFTI2C_SCL(port, 0);
  Delay_ticks(port->halfTicks); // data setup time & SCL_LOW & SMBus requirement
  if (result == SI2C_NACK)
    SoftI2C_Stop(port);
  return result;
}

// 8 SCL clock, just send a byte
void SoftI2C_SendByte(SoftI2C_Port *port, uint8_t byte) // barely send a byte
{
  int8_t i;
  SOFTI2C_SCL(port, 0);
  for (i = 7; i >= 0; i--)
  {
    SOFTI2C_SDA(port, (byte >> i) & 1u);
    Delay_ticks(port->halfTicks); // data setup time
    SOFTI2C_SCL(port, 1);
    Delay_ticks(port->delayTicks); // data hold time
    SOFTI2C_SCL(port, 0);
    Delay_ticks(port->halfTicks); // data setup time & SCL_LOW & SMBus requirement
  }
}

// 8 SCL clock, just read a byte
uint8_t SoftI2C_ReadByte(SoftI2C_Port *port) // barely read a byte
{
  uint8_t i, result = 0;
  SOFTI2C_SDA_IN(port);
  for (i = 0; i < 8; i++)
  {
    SOFTI2C_SCL(port, 0);
    Delay_ticks(port->delayTicks); // data setup time & SCL_LOW & SMBus requirement
    SOFTI2C_SCL(port, 1);
    result <<= 1;
    Delay_ticks(port->halfTicks); // data setup time
    result |= SOFTI2C_READSDA(port);
    Delay_ticks(port->halfTicks); // data hold time
  }
  return result;
}

// 9 SCL clock, treat the 9th bit as ACK/NACK
// If SI2C_NACK is given, it will always return true(ignore ACK/NACK)
// If SI2C_ACK is given, it will retry SOFTI2C_RETRYTIMES if no ACK is detected,
// and it will return whether the ACK is detected finally.
uint8_t SoftI2C_SendByte_ACK(SoftI2C_Port *port, uint8_t byte, uint8_t handleACK) // handle ACK and retry
{
  uint8_t i;
  if (handleACK == SI2C_NACK)
  {
    SoftI2C_SendByte(port, byte);
    SOFTI2C_SDA_IN(port);
    SOFTI2C_SCL(port, 0);
    Delay_ticks(port->halfTicks);  // data setup time
    SOFTI2C_SCL(port, 1);          // can be read
    Delay_ticks(port->delayTicks); // hold
    SOFTI2C_SCL(port, 0);          // cannot be read
    Delay_ticks(port->halfTicks);  // data setup time & SCL_LOW & SMBus requirement
    return 1;
  }
  else
  {
    for (i = 0; i < SOFTI2C_RETRYTIMES; i++)
    {
      SoftI2C_SendByte(port, byte);
      if (SoftI2C_WaitACK(port) == SI2C_ACK)
        break;
    }
    return (i < SOFTI2C_RETRYTIMES);
  }
}

// 9 SCL clock, send ACK/NACK after data received
uint8_t SoftI2C_ReadByte_ACK(SoftI2C_Port *port, uint8_t ACK)
{
  uint8_t result;
  result = SoftI2C_ReadByte(port);
  SoftI2C_SendACK(port, ACK);
  return result;
}

// return the number of available addresses, support 7bit/10bit address(depending on the port->addrLen)
uint16_t SoftI2C_SearchAddr(SoftI2C_Port *port, uint16_t start, uint16_t end, uint16_t *buf)
{
  uint16_t num = 0;
  if (port->addrLen == SI2C_ADDR_7b && end > 127)
    end = 127;
  else if (port->addrLen == SI2C_ADDR_7b && end > 1023)
    end = 1023;
  for (; start <= end; start++)
  {
    SoftI2C_Start(port);
    if (SoftI2C_SendAddr(port,start,SI2C_WRITE))
      buf[num++] = start;
    SoftI2C_Stop(port);
  }
  return num;
}