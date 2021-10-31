#include "rda5820ns.h"

SoftI2C_Port RDA5820_port;

uint16_t RDA5820_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  uint16_t id;
  SoftI2C_SetPort(&RDA5820_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&RDA5820_port, 400000, SI2C_ADDR_7b);
  RDA5820_WriteReg(0x02, 0x0002); // soft reset
  Delay_ms(50);
  RDA5820_WriteReg(0x02, 0xC001); // audio output on, mute off, power up
  Delay_ms(600);                  // wait for oscillator
  // magic Rx init sequence
  // RDA5820_WriteReg(0x03, 0x0000);
  // RDA5820_WriteReg(0x04, 0x0400);
  // RDA5820_WriteReg(0x05, 0x88EF);
  // RDA5820_WriteReg(0x14, 0x2000);
  // RDA5820_WriteReg(0x15, 0x88FE);
  // RDA5820_WriteReg(0x16, 0x4C00);
  // RDA5820_WriteReg(0x1C, 0x221c);
  // RDA5820_WriteReg(0x25, 0x0E1C);
  // RDA5820_WriteReg(0x27, 0xBB6C);
  // RDA5820_WriteReg(0x5C, 0x175C);
  // another magic Rx init sequence
  // RDA5820_WriteReg(0x05, 0x888F);  //LNAP  0x884F --LNAN
  // RDA5820_WriteReg(0x06, 0x6000);
  // RDA5820_WriteReg(0x13, 0x80E1);
  // RDA5820_WriteReg(0x14, 0x2A11);
  // RDA5820_WriteReg(0x1C, 0x22DE);
  // RDA5820_WriteReg(0x21, 0x0020);
  // RDA5820_WriteReg(0x03, 0x1B90);
  return RDA5820_ReadID();
}

uint8_t RDA5820_ReadReg(uint8_t reg, uint16_t *data)
{
  uint8_t tmp[2];
  if (!SoftI2C_Read(&RDA5820_port, RDA5820_ADDR, reg, tmp, 2))
    return 0;
  *data = ((uint16_t)tmp[0] << 8) | tmp[1];
  // printf("*****read:0x%x,0x%x\n", reg, *data);
  return 1;
}

uint8_t RDA5820_WriteReg(uint8_t reg, uint16_t data)
{
  uint8_t tmp[2] = {data >> 8, data & 0xFF};
  // printf("*****write:0x%x,0x%x\n", reg, data);
  return SoftI2C_Write(&RDA5820_port, RDA5820_ADDR, reg, tmp, 2);
}

uint16_t RDA5820_ReadID(void)
{
  uint16_t id = 0;
  if (!RDA5820_ReadReg(0x00, &id))
    return 0;
  return id;
}

uint8_t RDA5820_SetWorkMode(uint8_t workMode)
{
  uint16_t reg;
  if (!RDA5820_ReadReg(0x40, &reg))
    return 0;
  reg &= 0xFFF0;
  reg |= (workMode & 0xF);
  return RDA5820_WriteReg(0x40, reg);
}

uint8_t RDA5820_SetFreq(double freq) //50~115, in MHz
{
  uint16_t reg3, reg7;
  uint16_t chNb;
  uint8_t band, chSp, mode50;

  RDA5820_ReadReg(0x03, &reg3);
  chSp = reg3 & 0x3;
  if (chSp == 0)
    chSp = 10;
  else if (chSp == 1)
    chSp = 5;
  else if (chSp == 2)
    chSp = 20;
  else // chSp == 3
    chSp = 40;

  mode50 = 0x1;  // default
  if (freq < 65) // 50~65
  {
    if (freq < 50)
      freq = 50;
    mode50 = 0x0; // 50M start

    band = 0x3; // 50~65/65~76
    freq -= 50;
  }
  else if (freq < 76) // 65~76
  {
    band = 0x3; // 50~65/65~76
    freq -= 65;
  }
  else if (freq < 101) // 76~101
  {
    band = 0x2; // 76~108
    freq -= 76;
  }
  else if (freq < 112.5875) // 101~112.575(0.575---0.5875---0.600)
  {
    band = 0x0; // 87~108
    freq -= 87;
  }
  else // 112.575~115
  {
    if (freq > 115)
      freq = 115;

    band = 0x0; // 87~108
    freq -= 87;
  }
  chNb = freq * chSp + 0.5;

  RDA5820_ReadReg(0x07, &reg7);
  reg7 &= 0xFDFF;
  reg7 |= mode50 << 9;
  RDA5820_WriteReg(0x07, reg7);

  reg3 &= 0x23;                             //keep direct mode bit and channel space
  reg3 |= (band << 2 | 1 << 4 | chNb << 6); // tune
  RDA5820_WriteReg(0x03, reg3);
  printf("chNb, band, chSp, 50: %d, %d, %d, %d\r\n", chNb, band, 1000 / chSp, mode50);
  return 1; //TODO: Get tune status
}

uint8_t RDA5820_SetVolume(uint8_t volume) // 4bit, 0~15
{
  uint16_t reg;
  if (!RDA5820_ReadReg(0x05, &reg))
    return 0;
  reg &= 0xFFF0;
  reg |= (volume & 0xF);
  return RDA5820_WriteReg(0x05, reg);
}

uint8_t RDA5820_SetChannelSpace(uint8_t channelSpace)
{
  uint16_t reg;
  if (!RDA5820_ReadReg(0x03, &reg))
    return 0;
  reg &= ~0x0003;
  reg |= (channelSpace & 0x0003);
  return RDA5820_WriteReg(0x03, reg);
}