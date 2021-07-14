#include "ads1115.h"

SoftI2C_Port ADS1115_port;
uint16_t ADS1115_currConf;

void ADS1115_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  SoftI2C_SetPort(&ADS1115_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&ADS1115_port, 400000, SI2C_ADDR_7b);
  ADS1115_UpdateConf();
}

uint8_t ADS1115_UpdateConf(void)
{
  uint16_t conf;
  if(!ADS1115_ReadConf(&conf))
    return 0;
  ADS1115_currConf = conf & ~ADS1115_CONF_OS;
  return 1;
}

uint8_t ADS1115_ReadReg(uint8_t reg, uint16_t *data)
{
  uint8_t tmp[2];
  if (!SoftI2C_Read(&ADS1115_port, ADS1115_ADDR, reg, tmp, 2))
    return 0;
  *data = ((uint16_t)tmp[0] << 8) | tmp[1];
  return 1;
}

uint8_t ADS1115_WriteReg(uint8_t reg, uint16_t data)
{
  uint8_t tmp[2] = {data >> 8, data & 0xFF};
  return SoftI2C_Write(&ADS1115_port, ADS1115_ADDR, reg, tmp, 2);
}

uint8_t ADS1115_SetMux(uint16_t mux)
{
  uint16_t conf;
  conf = ADS1115_currConf;
  conf &= ~(uint16_t)ADS1115_CONF_MUX_MASK;
  conf |= mux;
  if(ADS1115_WriteConf(conf))
  {
    ADS1115_currConf = conf;
    return 1;
  }
  return 0;
}

uint8_t ADS1115_SetPGA(uint16_t pga)
{
  uint16_t conf;
  conf = ADS1115_currConf;
  conf &= ~(uint16_t)ADS1115_CONF_PGA_MASK;
  conf |= pga;
  if(ADS1115_WriteConf(conf))
  {
    ADS1115_currConf = conf;
    return 1;
  }
  return 0;
}

uint8_t ADS1115_SetDataRate(uint16_t dataRate)
{
  uint16_t conf;
  conf = ADS1115_currConf;
  conf &= ~(uint16_t)ADS1115_CONF_DR_MASK;
  conf |= dataRate;
  if(ADS1115_WriteConf(conf))
  {
    ADS1115_currConf = conf;
    return 1;
  }
  return 0;
}

uint8_t ADS1115_SetCompQueue(uint16_t compQueue)
{
  uint16_t conf;
  conf = ADS1115_currConf;
  conf &= ~(uint16_t)ADS1115_CONF_COMPQUEUE_MASK;
  conf |= compQueue;
  if(ADS1115_WriteConf(conf))
  {
    ADS1115_currConf = conf;
    return 1;
  }
  return 0;
}

uint8_t ADS1115_SetMode(uint16_t mode)
{
  uint16_t conf;
  conf = ADS1115_currConf;
  conf &= ~(uint16_t)ADS1115_CONF_MODE_MASK;
  conf |= mode;
  if(ADS1115_WriteConf(conf))
  {
    ADS1115_currConf = conf;
    return 1;
  }
  return 0;
}

void ADS1115_Start(void)
{
  ADS1115_WriteConf(ADS1115_currConf | ADS1115_CONF_OS);
}