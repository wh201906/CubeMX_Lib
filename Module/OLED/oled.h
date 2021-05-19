#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "DELAY/delay.h"
#include "I2C/softi2c.h"
#include "UTIL/util.h"

extern uint8_t OLED_cursorX, OLED_cursorY;

#define OLED_ADDRESS 0x3C

#define TEXTSIZE_SMALL 0
#define TEXTSIZE_BIG 1

#define REVERSE_OFF 0
#define REVERSE_ON 1

#define ROTATE_OFF 0
#define ROTATE_ON 1

// Note:
// the x is from 0~127(128 pixels)
// the y is from 0~7(8 pages)

static void WriteCmd(uint8_t command);
static void WriteData(uint8_t data);
static void FastWrite_Start(void);
static void FastWrite(uint8_t data);
static void FastWrite_Stop(void);
void OLED_Init(void);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_Fill(uint8_t data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
uint8_t OLED_ShowStr(uint8_t x, uint8_t y, uint8_t *str);
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t index);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch);
int OLED_ShowInt(uint8_t x, uint8_t y, int64_t val);
int OLED_ShowFloat(uint8_t x, uint8_t y, double val);
void OLED_SetBrightness(uint8_t val);
void OLED_SetGlobalReverse(uint8_t state);
void OLED_SetCurrentReverse(uint8_t state);
void OLED_SetTextSize(uint8_t size);
void OLED_Rotate(uint8_t state);

#endif
