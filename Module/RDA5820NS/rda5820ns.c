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
  Delay_ms(600);
  return RDA5820_ReadID();
}

uint8_t RDA5820_ReadReg(uint8_t reg, uint16_t *data)
{
  uint8_t tmp[2];
  if (!SoftI2C_Read(&RDA5820_port, RDA5820_ADDR, reg, tmp, 2))
    return 0;
  *data = ((uint16_t)tmp[0] << 8) | tmp[1];
  return 1;
}

uint8_t RDA5820_WriteReg(uint8_t reg, uint16_t data)
{
  uint8_t tmp[2] = {data >> 8, data & 0xFF};
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

uint8_t RDA5820_SetFreq(double freq) //50~115, in MHz, maximum precision
{
  uint16_t reg;
  uint16_t chNb;
  uint8_t band, chSp, mode50;

  mode50 = 0x1;  // default
  chSp = 0x3;    // 25k
  if (freq < 65) // 50~65
  {
    if (freq < 50)
      freq = 50;
    mode50 = 0x0; // 50M start

    band = 0xC; // 50~65/65~76
    freq -= 50;
    chNb = freq * 40 + 0.5;
  }
  else if (freq < 76) // 65~76
  {
    band = 0xC; // 50~65/65~76
    freq -= 65;
    chNb = freq * 40 + 0.5;
  }
  else if (freq < 101) // 76~101
  {
    band = 0x8; // 76~108
    freq -= 76;
    chNb = freq * 40 + 0.5;
  }
  else if (freq < 112.5875) // 101~112.575(0.575---0.5875---0.600)
  {
    band = 0x0; // 87~108
    freq -= 87;
    chNb = freq * 40 + 0.5;
  }
  else // 112.575~115
  {
    if (freq > 115)
      freq = 115;
    chSp = 0x2; // 50k

    band = 0x0; // 87~108
    freq -= 87;
    chNb = freq * 20 + 0.5;
  }
  RDA5820_ReadReg(0x07, &reg);
  reg &= 0xFDFF;
  reg |= mode50;
  RDA5820_WriteReg(0x07, reg);
  RDA5820_ReadReg(0x03, &reg);
  reg &= 0x20;                               //keep direct mode bit
  reg |= (chSp | band | 0x10 | (chNb << 6)); // tune
  RDA5820_WriteReg(0x03, reg);
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