#ifndef _MYVL53L1X_H
#define _MYVL53L1X_H

#include "API/VL53L1X_calibration.h"
#include "API/vl53l1_platform.h"
#include "API/vl53l1_types.h"
#include "API/VL53L1X_api.h"
#include "I2C/softi2c.h"
#include "main.h"

#define MYVL53L1X_DEFAULT_ADDR 0x29

uint8_t MyVL53L1X_Init(uint8_t deviceAddr, GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID);
int32_t MyVL53L1X_GetDistance(void);

#endif