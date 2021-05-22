#include "myvl53l1x.h"

VL53L1_DEV VL53L1_Inst;

uint8_t MyVL53L1X_Init(uint8_t deviceAddr, GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  uint8_t i, state = 0;
  VL53L1_Inst = deviceAddr;
  SoftI2C_SetPort(&VL53L1X_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&VL53L1X_port, 100000, SI2C_ADDR_7b);
  for (i = 0; i < 50; i++)
  {
    Delay_ms(200);
    VL53L1X_BootState(VL53L1_Inst, &state);
    if (state & 0x1)
      break;
  }
  if (i >= 50)
    return 0; // failed to boot
  Delay_ms(100);
  if (VL53L1X_SensorInit(VL53L1_Inst) != VL53L1X_ERROR_NONE)
    return 0;
  return 1;
}

int32_t MyVL53L1X_GetDistance(void)
{
  uint16_t i, distance;
  uint8_t status;
  uint8_t isReady = 0;
  if (VL53L1X_StartRanging(VL53L1_Inst))
    return -1;
  for (i = 0; i < 500; i++)
  {
    Delay_ms(1);
    VL53L1X_CheckForDataReady(VL53L1_Inst, &isReady);
    if (isReady)
      break;
  }
  Delay_ms(200);
  if (i >= 500)
    return -1;
  if (VL53L1X_GetDistance(VL53L1_Inst, &distance) != VL53L1X_ERROR_NONE)
    return -1;
  VL53L1X_ClearInterrupt(VL53L1_Inst);
  VL53L1X_StopRanging(VL53L1_Inst);
  return distance;
}