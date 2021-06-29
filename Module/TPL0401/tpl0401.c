#include "tpl0401.h"

uint8_t TPL0401_addr = TPL0401_ADDR_A_C;

SoftI2C_Port TPL0401_port;

// the wiper will be at mid scale after powerd up (5kOhm, 0x40 for TPL0401A/B)
void TPL0401_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID, char type)
{
  SoftI2C_SetPort(&TPL0401_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&TPL0401_port, 100000, SI2C_ADDR_7b);

  if (type == 'B' || type == 'b') // TPL0401B
    TPL0401_addr = TPL0401_ADDR_B;
  else // TPL0401A/TPL0401C
    TPL0401_addr = TPL0401_ADDR_A_C;
}

uint8_t TPL0401_WriteVal(uint8_t data)
{
  return SoftI2C_Write(&TPL0401_port, TPL0401_addr, TPL0401_REG, &data, 1);
}
uint8_t TPL0401_ReadVal(void) // the value should be in [0:127], otherwise the value is invalid(failed to read)
{
  uint8_t res;
  if (!SoftI2C_Read(&TPL0401_port, TPL0401_addr, TPL0401_REG, &res, 1))
    res = 0xFF; // invalid
  return res;
}
