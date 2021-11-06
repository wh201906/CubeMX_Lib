#include "tm1637.h"

SoftI2C_Port TM1637_port;
uint16_t TM1637_currConf;

uint8_t TM1637_digit[17] =
    {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x00};

// NOTE:
// The bit order is reversed in data sheet

void TM1637_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  SoftI2C_SetPort(&TM1637_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&TM1637_port, 100000, SI2C_ADDR_7b);
}

uint8_t TM1637_Write(uint8_t *data, uint16_t len)
{
  uint16_t i;
  uint8_t tmp;
  Delay_us(1);
  SoftI2C_Start(&TM1637_port);
  for (i = 0; i < len; i++)
  {
    tmp = (data[i] * 0x0202020202ull & 0x010884422010ull) % 0x3FFu; // reverse bit order
    if (!SoftI2C_SendByte_ACK(&TM1637_port, tmp, SI2C_NACK))
      return 0;
  }
  SoftI2C_Stop(&TM1637_port);
  return 1;
}

void TM1637_SetBrightness(uint8_t val) // 0~8
{
  val %= 9; // 0~8
  val += (TM1637_DISPLAY_HEAD | TM1637_DISPLAY_ON) - 1;
  TM1637_Write(&val, 1);
}

void TM1637_SetNum(uint8_t *val) // 4 digit, 0~15 to show, 16~255->off
{
  uint8_t buf[6] = {0};
  buf[0] = TM1637_CMD_HEAD | TM1637_CMD_FUNC_WRITE;
  buf[1] = TM1637_ADDR_HEAD | 0;
  buf[2] = val[0] > 15 ? TM1637_digit[16] : TM1637_digit[val[0]];
  buf[3] = val[1] > 15 ? TM1637_digit[16] : TM1637_digit[val[1]];
  buf[4] = val[2] > 15 ? TM1637_digit[16] : TM1637_digit[val[2]];
  buf[5] = val[3] > 15 ? TM1637_digit[16] : TM1637_digit[val[3]];
  TM1637_Write(buf, 1);
  TM1637_Write(buf+1, 5);
}