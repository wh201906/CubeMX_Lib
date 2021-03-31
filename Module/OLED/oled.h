#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "DELAY/delay.h"
#include "I2C/softi2c1.h"
#include "UTIL/util.h"

#define OLED_ADDRESS 0x3C

#define TEXTSIZE_SMALL 0
#define TEXTSIZE_BIG 1

#define REVERSE_OFF 0
#define REVERSE_ON 1

void WriteCmd(uint8_t I2C_Command);
void WriteDat(uint8_t I2C_Data, uint8_t reverse);
void OLED_Init(void);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_Fill(uint8_t fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(uint8_t x, uint8_t y, uint8_t ch[]);
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t index);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[]);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch);
int OLED_ShowInt(uint8_t x, uint8_t y, int64_t val);
int OLED_ShowFloat(uint8_t x, uint8_t y, double val);
void OLED_SetBrightness(uint8_t val);
void OLED_SetGlobalReverse(uint8_t state);
void OLED_SetCurrentReverse(uint8_t state);
void OLED_SetTextSize(uint8_t size);

#endif
