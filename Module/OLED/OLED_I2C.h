#ifndef __OLED_I2C_H
#define	__OLED_I2C_H

#include "main.h"
#include "DELAY/delay.h"
#include "OLED/myiic.h"
#include "stdio.h"
#include "string.h"

extern uint8_t RevState;
extern uint8_t brightness;

#define OLED_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78

#define TEXTSIZE_SMALL 1
#define TEXTSIZE_BIG 2

#define REVERSE_OFF 0
#define REVERSE_ON 1


void I2C_WriteByte(uint8_t addr,uint8_t data);
void WriteCmd(uint8_t I2C_Command);
void WriteDat(uint8_t I2C_Data, uint8_t reverse);
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_SetBrightness(unsigned char val);
void OLED_Reverse(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize, uint8_t reverse);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char index, uint8_t reverse);
void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch, unsigned char TextSize, uint8_t reverse);
int OLED_ShowInt(unsigned char x, unsigned char y, int val, unsigned char TextSize, uint8_t reverse);
int OLED_ShowFloat(unsigned char x, unsigned char y, double val, unsigned char TextSize, uint8_t reverse);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[], uint8_t reverse);
#endif
