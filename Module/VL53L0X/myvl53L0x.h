#ifndef _MYVL53L0X_H
#define _MYVL53L0X_H

#define VL53L0X_SENSE_DEFAULT 0
#define VL53L0X_SENSE_LONG_RANGE 1
#define VL53L0X_SENSE_HIGH_SPEED 2
#define VL53L0X_SENSE_HIGH_ACCURACY 3
// #define VL53L0X_Sense_config_t ;

#include "API/core/inc/vl53l0x_api.h"
#include "API/core/inc/vl53l0x_api_core.h"
#include "API/platform/inc/vl53l0x_i2c_platform.h"
#include "I2C/softi2c2.h"
#include "main.h"

uint8_t MyVL53L0X_Init(uint8_t deviceAddr);
uint8_t MyVL53L0X_SetSenseMode(uint8_t senseMode);
uint8_t MyVL53L0X_SingleMeasure(VL53L0X_RangingMeasurementData_t *RangingMeasurementData);
uint16_t MyVL53L0X_GetDistance(void);

#endif