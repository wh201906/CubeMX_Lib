#ifndef _MYVL6180X_H
#define _MYVL6180X_H

#include "API/vl6180x_cfg.h"
#include "API/vl6180x_def.h"
#include "API/vl6180x_platform.h"
#include "API/vl6180x_types.h"
#include "API/vl6180x_api.h"
#include "I2C/softi2c.h"
#include "main.h"

#define MYVL6180X_DEFAULT_ADDR 0x29

uint8_t MyVL6180X_Init(uint8_t deviceAddr);
int32_t MyVL6180X_GetDistance(void);
int64_t MyVL6180X_GetLux(void);

#endif