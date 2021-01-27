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

void SoftI2C1_Start(void)
{
  SOFTI2C1_SDA_OUT();
  SOFTI2C1_SCL(1);
  SOFTI2C1_SDA(1);
  Delay_ticks(delayTicks); // setup time
  SOFTI2C1_SDA(0);         // START: when CLK is high,DATA change form HIGH to LOW
  Delay_ticks(delayTicks); // hold time

  SOFTI2C1_SCL(0); // cannot be read
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

void SoftI2C1_SendACK(uint8_t isACK) // 1:ACK 0:NACK
{
  SOFTI2C1_SCL(0); // change start
  SOFTI2C1_SDA_OUT();
  SOFTI2C1_SDA(!isACK);        // 1:ACK 0:NACK
  Delay_ticks(delayTicks / 8); // data setup time
  SOFTI2C1_SCL(1);             // can be read
  Delay_ticks(delayTicks);     // hold
  SOFTI2C1_SCL(0);             // cannot be read
  Delay_ticks(delayTicks);     // data setup time & SCL_LOW
}

uint8_t SoftI2C1_WaitACK(void) // 1:ACK 0:NACK/No response
{
  uint16_t waitTime = 0;
  SOFTI2C1_SDA_IN();
  SOFTI2C1_SCL(1);
  Delay_ticks(delayTicks / 8); // data setup time
  while (SOFTI2C1_READSDA())
  {
    __NOP();
    waitTime++;
    if (waitTime > delayTicks)
    {
      SoftI2C1_Stop();
      return 0;
    }
  }
  SOFTI2C1_SCL(0);
  return 1;
}

void SoftI2C1_SendByte(uint8_t byte)
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
    Delay_ticks(delayTicks); // data setup time & SCL_LOW
  }
}

uint8_t SoftI2C1_ReadByte(uint8_t sendACK) // 1: send ACK 0: no ACK
{
  uint8_t i, result = 0;
  SOFTI2C1_SDA_IN();
  for (i = 0; i < 8; i++)
  {
    SOFTI2C1_SCL(0);
    Delay_ticks(delayTicks); // data setup time & SCL_LOW
    SOFTI2C1_SCL(1);
    result <<= 1;
    Delay_ticks(delayTicks / 8); // data setup time
    result |= SOFTI2C1_READSDA() & 1u;
    Delay_ticks(delayTicks); // data hold time
  }
  SoftI2C1_SendACK(sendACK);
  return result;
}