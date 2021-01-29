#ifndef __OLED_I2C_H
#define __OLED_I2C_H

#include "main.h"
#include "DELAY/delay.h"
#include "I2C/softi2c1.h"
#include "stdio.h"
#include "string.h"

extern uint8_t RevState;
extern uint8_t brightness;

#define OLED_ADDRESS 0x78

#define TEXTSIZE_SMALL 1
#define TEXTSIZE_BIG 2

#define REVERSE_OFF 0
#define REVERSE_ON 1

void I2C_WriteByte(uint8_t addr, uint8_t data);
void WriteCmd(uint8_t I2C_Command);
void WriteDat(uint8_t I2C_Data, uint8_t reverse);
void OLED_Init(void);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_Fill(uint8_t fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_SetBrightness(uint8_t val);
void OLED_Reverse(void);
void OLED_ShowStr(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize, uint8_t reverse);
void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t index, uint8_t reverse);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch, uint8_t TextSize, uint8_t reverse);
int OLED_ShowInt(uint8_t x, uint8_t y, int val, uint8_t TextSize, uint8_t reverse);
int OLED_ShowFloat(uint8_t x, uint8_t y, double val, uint8_t TextSize, uint8_t reverse);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[], uint8_t reverse);
#endif
