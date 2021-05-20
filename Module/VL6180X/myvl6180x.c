#include "myvl6180x.h"

VL6180xDev_t VL6180X_Inst;

uint8_t MyVL6180X_Init(uint8_t deviceAddr)
{
  SoftI2C_SetPort(&VL6180X_port, GPIOB, 3, GPIOB, 5);
  SoftI2C_Init(&VL6180X_port, 100000, SI2C_ADDR_7b);
  if (VL6180x_InitData(VL6180X_Inst) != 0)
    return 0;
  if (VL6180x_Prepare(VL6180X_Inst) != 0)
    return 0;
  return 1;
}

int32_t MyVL6180X_GetDistance(void)
{
  VL6180x_RangeData_t Range;
  VL6180x_RangePollMeasurement(VL6180X_Inst, &Range);
  if (Range.errorStatus == 0)
    return Range.range_mm;
  else
    return -1;
}