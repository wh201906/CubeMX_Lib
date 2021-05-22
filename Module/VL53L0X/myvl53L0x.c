#include "myvl53L0x.h"

VL53L0X_Dev_t VL53L0X_Inst;
VL53L0X_DeviceInfo_t VL53L0X_DeviceInfo;
VL53L0X_Error Status = VL53L0X_ERROR_NONE;

uint8_t MyVL53L0X_Init(uint8_t deviceAddr, GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  uint32_t refSpadCount;
  uint8_t isApertureSpads;
  uint8_t VhvSettings;
  uint8_t PhaseCal;

  VL53L0X_Inst.I2cDevAddr = deviceAddr;
  VL53L0X_Inst.comms_type = 0;
  VL53L0X_Inst.comms_speed_khz = 400;
  SoftI2C_SetPort(&VL53L0X_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&VL53L0X_port, VL53L0X_Inst.comms_speed_khz, SI2C_ADDR_7b);

  Status = VL53L0X_DataInit(&VL53L0X_Inst);
  Status = VL53L0X_GetDeviceInfo(&VL53L0X_Inst, &VL53L0X_DeviceInfo);
  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_StaticInit(&VL53L0X_Inst);
  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_PerformRefSpadManagement(&VL53L0X_Inst, &refSpadCount, &isApertureSpads);
  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_PerformRefCalibration(&VL53L0X_Inst, &VhvSettings, &PhaseCal);
  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_SetDeviceMode(&VL53L0X_Inst, VL53L0X_DEVICEMODE_SINGLE_RANGING);
  if (Status == VL53L0X_ERROR_NONE)
    MyVL53L0X_SetSenseMode(VL53L0X_SENSE_DEFAULT);

  return (Status == VL53L0X_ERROR_NONE);
}

uint8_t MyVL53L0X_SetSenseMode(uint8_t senseMode)
{
  Status = VL53L0X_SetLimitCheckEnable(&VL53L0X_Inst, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
  if (Status == VL53L0X_ERROR_NONE)
    Status = VL53L0X_SetLimitCheckEnable(&VL53L0X_Inst, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);

  if (Status != VL53L0X_ERROR_NONE)
    return 0;

  if (senseMode == VL53L0X_SENSE_DEFAULT)
  {
    Status = VL53L0X_SetLimitCheckEnable(&VL53L0X_Inst, VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, 1);
    if (Status == VL53L0X_ERROR_NONE)
      Status = VL53L0X_SetLimitCheckValue(&VL53L0X_Inst, VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, (FixPoint1616_t)(1.5 * 0.023 * 65536));
  }
  else if (senseMode == VL53L0X_SENSE_LONG_RANGE)
  {
    Status = VL53L0X_SetLimitCheckValue(&VL53L0X_Inst, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.1 * 65536));
    if (Status == VL53L0X_ERROR_NONE)
      Status = VL53L0X_SetLimitCheckValue(&VL53L0X_Inst, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(60 * 65536));
    if (Status == VL53L0X_ERROR_NONE)
      Status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&VL53L0X_Inst, 33000);
    if (Status == VL53L0X_ERROR_NONE)
      Status = VL53L0X_SetVcselPulsePeriod(&VL53L0X_Inst, VL53L0X_VCSEL_PERIOD_PRE_RANGE, 18);
    if (Status == VL53L0X_ERROR_NONE)
      Status = VL53L0X_SetVcselPulsePeriod(&VL53L0X_Inst, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 14);
  }
  else if (senseMode == VL53L0X_SENSE_HIGH_SPEED)
  {
    Status = VL53L0X_SetLimitCheckValue(&VL53L0X_Inst, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.25 * 65536));
    if (Status == VL53L0X_ERROR_NONE)
      Status = VL53L0X_SetLimitCheckValue(&VL53L0X_Inst, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(32 * 65536));
    if (Status == VL53L0X_ERROR_NONE)
      Status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&VL53L0X_Inst, 30000);
  }
  else if (senseMode == VL53L0X_SENSE_HIGH_ACCURACY)
  {
    Status = VL53L0X_SetLimitCheckValue(&VL53L0X_Inst, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.25 * 65536));
    if (Status == VL53L0X_ERROR_NONE)
      Status = VL53L0X_SetLimitCheckValue(&VL53L0X_Inst, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(18 * 65536));
    if (Status == VL53L0X_ERROR_NONE)
      Status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&VL53L0X_Inst, 200000);
    if (Status == VL53L0X_ERROR_NONE)
      Status = VL53L0X_SetLimitCheckEnable(&VL53L0X_Inst, VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, 0);
  }
  return (Status == VL53L0X_ERROR_NONE);
}

uint8_t MyVL53L0X_SingleMeasure(VL53L0X_RangingMeasurementData_t *RangingMeasurementData)
{
  Status = VL53L0X_ERROR_NONE;
  FixPoint1616_t LimitCheckCurrent;
  Status = VL53L0X_PerformSingleRangingMeasurement(&VL53L0X_Inst, RangingMeasurementData);
  return (Status == VL53L0X_ERROR_NONE);
}

uint16_t MyVL53L0X_GetDistance(void)
{
  VL53L0X_RangingMeasurementData_t RangingMeasurementData;
  if (MyVL53L0X_SingleMeasure(&RangingMeasurementData) && RangingMeasurementData.RangeStatus != 4)
    return RangingMeasurementData.RangeMilliMeter;
  else
    return 0xFFFF;
}