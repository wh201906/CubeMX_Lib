#include "ch423.h"

SoftI2C_Port CH423_port;

// OC[7:0], OC[15:8], IO[7:0]
uint8_t CH423_portState[3] = {0xFF, 0xFF, 0xFF};

// working
// full bright with limited current
// push pull for OCPin
// interrupt disabled
// scanH disabled
// scanL disabled
// output for IOPin
uint8_t CH423_config = 0x01;

void CH423_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  SoftI2C_SetPort(&CH423_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&CH423_port, 400000, SI2C_ADDR_7b); // maximum clock is 5MHz
}

void CH423_Write(uint8_t cmd, uint8_t data)
{
  SoftI2C_Start(&CH423_port);
  SoftI2C_SendByte(&CH423_port, cmd);
  SoftI2C_SendACK(&CH423_port, SI2C_NACK);
  SoftI2C_SendByte(&CH423_port, data);
  SoftI2C_SendACK(&CH423_port, SI2C_NACK);
  SoftI2C_Stop(&CH423_port);
}

uint8_t CH423_Read(uint8_t cmd)
{
  uint8_t result;
  SoftI2C_Start(&CH423_port);
  SoftI2C_SendByte(&CH423_port, cmd);
  SoftI2C_SendACK(&CH423_port, SI2C_NACK);
  result = SoftI2C_ReadByte(&CH423_port);
  SoftI2C_SendACK(&CH423_port, SI2C_NACK);
  SoftI2C_Stop(&CH423_port);
  return result;
}

// data[24:16]: IO[7:0]
// data[15:8]: OC[15:8]
// data[7:0]: OC[7:0]
void CH423_WriteAll(uint32_t data)
{
  CH423_portState[0] = data >> 0 & 0xFF;
  CH423_portState[1] = data >> 8 & 0xFF;
  CH423_portState[2] = data >> 16 & 0xFF;

  CH423_Write(CH423_OCL_W, CH423_portState[0]);
  CH423_Write(CH423_OCH_W, CH423_portState[1]);
  CH423_Write(CH423_IO_W, CH423_portState[2]);
}

void CH423_WriteOC(uint16_t data)
{
  CH423_portState[0] = data >> 0 & 0xFF;
  CH423_portState[1] = data >> 8 & 0xFF;

  CH423_Write(CH423_OCL_W, CH423_portState[0]);
  CH423_Write(CH423_OCH_W, CH423_portState[1]);
}

void CH423_WritePin(uint8_t id, uint8_t state)
{
  uint8_t grp = id / 8;
  id -= grp * 8;
  if (state)
    CH423_portState[grp] |= (uint8_t)(1 << id);
  else
    CH423_portState[grp] &= ~(uint8_t)(1 << id);

  if (grp == 0)
    CH423_Write(CH423_OCL_W, CH423_portState[0]);
  else if (grp == 1)
    CH423_Write(CH423_OCH_W, CH423_portState[1]);
  else if (grp == 2)
    CH423_Write(CH423_IO_W, CH423_portState[2]);
  else
    return;
}

uint8_t CH423_ReadPin(uint8_t id)
{
  uint8_t grp = id / 8;
  id -= grp * 8;
  if (grp > 2)
    return 0xFF;
  else if (grp == 2)
    CH423_portState[2] = CH423_Read(CH423_IO_R);
  return CH423_portState[grp] >> id & 0x1;
}

// CH423_IOPIN_IN / CH423_IOPIN_OUT
void CH423_SetIOPinMode(uint8_t mode)
{
  mode &= CH423_IOPIN_MASK;
  CH423_config &= ~CH423_IOPIN_MASK;
  CH423_config |= mode;
  CH423_Write(CH423_CONF, CH423_config);
}

// CH423_OCPIN_PUSHPULL / CH423_OCPIN_OPENDRAIN
void CHD23_SetOCPinMode(uint8_t mode)
{
  mode &= CH423_OCPIN_MASK;
  CH423_config &= ~CH423_OCPIN_MASK;
  CH423_config |= mode;
  CH423_Write(CH423_CONF, CH423_config);
}