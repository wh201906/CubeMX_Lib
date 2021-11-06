#ifndef _TM1637_H
#define _TM1637_H

#include "main.h"
#include "I2C/softi2c.h"
#include "DELAY/delay.h"

#define TM1637_CMD_HEAD (1u << 6)
#define TM1637_CMD_FUNC_WRITE 0
#define TM1637_CMD_FUNC_READ (1u << 1)
#define TM1637_CMD_ADDR_INC 0
#define TM1637_CMD_ADDR_FIXED (1u << 2)

#define TM1637_ADDR_HEAD (3u << 6)

#define TM1637_DISPLAY_HEAD (2u << 6)
#define TM1637_DISPLAY_OFF 0
#define TM1637_DISPLAY_ON (1u << 3)

void TM1637_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID);
uint8_t TM1637_Write(uint8_t *data, uint16_t len);
void TM1637_Test();
void TM1637_SetBrightness(uint8_t val); // 0~8
void TM1637_SetNum(uint8_t *val); // 4 digit, 0~15 to show, 16~255->off

#endif