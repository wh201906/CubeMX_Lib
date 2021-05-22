#include "myvl6180x.h"

VL6180xDev_t VL6180X_Inst;

uint8_t MyVL6180X_Init(uint8_t deviceAddr, GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  SoftI2C_SetPort(&VL6180X_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&VL6180X_port, 100000, SI2C_ADDR_7b);
  VL6180X_Inst = deviceAddr;
  if (VL6180x_InitData(VL6180X_Inst) != 0)
    return 0;
  if (VL6180x_Prepare(VL6180X_Inst) != 0)
    return 0;
  return 1;
}

int32_t MyVL6180X_GetDistance(void) // the orignal result is int32_t. I guess the range cannot be negative.
{
  VL6180x_RangeData_t Range;
  VL6180x_RangePollMeasurement(VL6180X_Inst, &Range);
  if (Range.errorStatus == 0)
    return Range.range_mm;
  else
    return -1;
}

int64_t MyVL6180X_GetLux(void) // the orignal result is uint32_t, I use negative number to indicate whether there is a error.
{
  VL6180x_AlsData_t Als;
  VL6180x_AlsPollMeasurement(VL6180X_Inst, &Als);
  if (Als.errorStatus == 0)
    return Als.lux;
  else
    return -1;
}