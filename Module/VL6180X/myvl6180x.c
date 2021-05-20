#include "myvl6180x.h"

VL6180xDev_t VL6180X_Inst;

uint8_t MyVL6180X_Init(uint8_t deviceAddr)
{
  SoftI2C_SetPort(&VL6180X_port, GPIOB, 3, GPIOB, 4);
  SoftI2C_Init(&VL6180X_port, 100000, SI2C_ADDR_7b);
  VL6180x_InitData(VL6180X_Inst);
  VL6180x_Prepare(VL6180X_Inst);
}