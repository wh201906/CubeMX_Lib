
#include "OLED/codetab.h"
#include "OLED/oled.h"

uint8_t isGlobalReverse = REVERSE_ON;
uint8_t isCurrentReverse = REVERSE_OFF;
uint8_t textSize = TEXTSIZE_SMALL;

uint8_t OLED_cursorX, OLED_cursorY;

void WriteCmd(uint8_t I2C_Command) //写命令
{
  SoftI2C1_Write(OLED_ADDRESS, SI2C_ADDR_7b, 0x00, &I2C_Command, 1);
}

void WriteData(uint8_t I2C_Data) //写数据
{
  if (!isCurrentReverse)
    SoftI2C1_Write(OLED_ADDRESS, SI2C_ADDR_7b, 0x40, &I2C_Data, 1);
  else
  {
    I2C_Data = ~I2C_Data;
    SoftI2C1_Write(OLED_ADDRESS, SI2C_ADDR_7b, 0x40, &I2C_Data, 1);
  }
}

void OLED_Init(void)
{
  SoftI2C1_Init(400000);
  Delay_ms(100); //这里的延时很重要

  WriteCmd(0xAE); //display off
  WriteCmd(0x20); //Set Memory Addressing Mode
  WriteCmd(0x00); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
  WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
  WriteCmd(0xc8); //Set COM Output Scan Direction
  WriteCmd(0x00); //---set low column address
  WriteCmd(0x10); //---set high column address
  WriteCmd(0x40); //--set start line address
  OLED_SetBrightness(0xFF);
  WriteCmd(0xa1); //--set segment re-map 0 to 127
  OLED_SetGlobalReverse(REVERSE_OFF);
  WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
  WriteCmd(0x3F); //
  WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
  WriteCmd(0xd3); //-set display offset
  WriteCmd(0x00); //-not offset
  WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
  WriteCmd(0xf0); //--set divide ratio
  WriteCmd(0xd9); //--set pre-charge period
  WriteCmd(0x22); //
  WriteCmd(0xda); //--set com pins hardware configuration
  WriteCmd(0x12);
  WriteCmd(0xdb); //--set vcomh
  WriteCmd(0x20); //0x20,0.77xVcc
  WriteCmd(0x8d); //--set DC-DC enable
  WriteCmd(0x14); //
  WriteCmd(0xaf); //--turn on oled panel
  OLED_CLS();
}

void OLED_SetPos(uint8_t x, uint8_t y) //设置起始点坐标
{
  WriteCmd(0xb0 + y);
  WriteCmd(((x & 0xf0) >> 4) | 0x10);
  WriteCmd((x & 0x0f) | 0x01);
}

void OLED_Fill(uint8_t fill_Data) //全屏填充
{
  uint8_t m, n;
  for (m = 0; m < 8; m++)
  {
    WriteCmd(0xb0 + m); //page0-page1
    WriteCmd(0x00);     //low column start address
    WriteCmd(0x10);     //high column start address
    for (n = 0; n < 128; n++)
    {

      WriteData(fill_Data);
    }
  }
}

void OLED_CLS(void) //清屏
{
  OLED_Fill(0x00);
}

void OLED_ON(void)
{
  WriteCmd(0X8D); //设置电荷泵
  WriteCmd(0X14); //开启电荷泵
  WriteCmd(0XAF); //OLED唤醒
}

void OLED_OFF(void)
{
  WriteCmd(0X8D); //设置电荷泵
  WriteCmd(0X10); //关闭电荷泵
  WriteCmd(0XAE); //OLED休眠
}

uint8_t OLED_ShowStr(uint8_t x, uint8_t y, uint8_t ch[])
{
  uint8_t c = 0, i = 0, j = 0;
  OLED_SetPos(x, y);
  if (textSize == TEXTSIZE_SMALL)
  {
    while (ch[j] != '\0')
    {
      c = ch[j] - ' ';
      if (x > 122)
      {
        x = 0;
        y++;
        OLED_SetPos(x, y);
      }
      for (i = 0; i < 6; i++)
        WriteData(F6x8[c][i]);
      x += 6;
      j++;
    }
  }
  else if (textSize == TEXTSIZE_BIG)
  {
    while (ch[j] != '\0')
    {
      c = ch[j] - ' ';
      if (x > 120)
      {
        x = 0;
        y++;
      }
      OLED_SetPos(x, y * 2);
      for (i = 0; i < 8; i++)
        WriteData(F8X16[c * 16 + i]);
      OLED_SetPos(x, y * 2 + 1);
      for (i = 0; i < 8; i++)
        WriteData(F8X16[c * 16 + i + 8]);
      x += 8;
      j++;
    }
  }

  if (textSize == TEXTSIZE_SMALL || textSize == TEXTSIZE_BIG)
  {
    OLED_cursorX = x;
    OLED_cursorY = y;
  }
  return j;
}

void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t index)
{
  uint8_t wm = 0;
  unsigned int adder = 32 * index;
  OLED_SetPos(x, y);
  for (wm = 0; wm < 16; wm++)
  {
    WriteData(F16x16[adder]);
    adder += 1;
  }
  OLED_SetPos(x, y + 1);
  for (wm = 0; wm < 16; wm++)
  {
    WriteData(F16x16[adder]);
    adder += 1;
  }
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch)
{
  uint8_t i = 0;
  if (textSize == TEXTSIZE_SMALL)
  {
    ch -= ' ';
    OLED_SetPos(x, y);
    for (i = 0; i < 6; i++)
      WriteData(F6x8[ch][i]);
  }
  else if (textSize == TEXTSIZE_BIG)
  {
    ch -= ' ';
    OLED_SetPos(x, y * 2);
    for (i = 0; i < 8; i++)
      WriteData(F8X16[ch * 16 + i]);
    OLED_SetPos(x, y + 1);
    for (i = 0; i < 8; i++)
      WriteData(F8X16[ch * 16 + 8 + i]);
  }
}

int OLED_ShowInt(uint8_t x, uint8_t y, int64_t val)
{
  uint8_t str[22], len;
  len = myitoa(val, str, 10);
  OLED_ShowStr(x, y, str);
  return len;
}

int OLED_ShowFloat(uint8_t x, uint8_t y, double val)
{
  uint8_t str[30], len;
  len = myftoa(val, str);
  OLED_ShowStr(x, y, str);
  return (len);
}

void OLED_SetBrightness(uint8_t val)
{
  WriteCmd(0x81);
  WriteCmd(val);
}

void OLED_SetGlobalReverse(uint8_t state)
{
  if (isGlobalReverse)
    WriteCmd(0xa6);
  else
    WriteCmd(0xa7);
}

void OLED_SetCurrentReverse(uint8_t state)
{
  isCurrentReverse = !!state;
}

void OLED_SetTextSize(uint8_t size)
{
  textSize = size;
}