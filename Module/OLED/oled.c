#include "OLED/codetab.h"
#include "OLED/oled.h"

uint8_t isCurrentReverse = REVERSE_OFF;
uint8_t textSize = TEXTSIZE_SMALL;
uint8_t isScrolling = 0;

uint8_t OLED_cursorX = 0, OLED_cursorY = 0;

SoftI2C_Port OLED_port;

static void WriteCmd(uint8_t command)
{
  SoftI2C_Write(&OLED_port, OLED_ADDRESS, 0x00, &command, 1);
}

static void WriteData(uint8_t data)
{
  if (!isCurrentReverse)
    SoftI2C_Write(&OLED_port, OLED_ADDRESS, 0x40, &data, 1);
  else
  {
    data = ~data;
    SoftI2C_Write(&OLED_port, OLED_ADDRESS, 0x40, &data, 1);
  }
}

static void FastWrite_Start(void)
{
  SoftI2C_Start(&OLED_port);
  SoftI2C_SendAddr(&OLED_port, OLED_ADDRESS, SI2C_WRITE);
  SoftI2C_SendByte_ACK(&OLED_port, 0x40, SI2C_ACK);
}

static void FastWrite(uint8_t data)
{
  SoftI2C_SendByte_ACK(&OLED_port, data, SI2C_ACK);
}

static void FastWrite_Stop(void)
{
  SoftI2C_Stop(&OLED_port);
}

void OLED_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  SoftI2C_SetPort(&OLED_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&OLED_port, 400000, SI2C_ADDR_7b);
  Delay_ms(100); //这里的延时很重要

  WriteCmd(0xAE); //display off

  WriteCmd(0x20); //Set Memory Addressing Mode
  WriteCmd(0x00);

  WriteCmd(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
  WriteCmd(0x00); //---set low column address
  WriteCmd(0x10); //---set high column address
  WriteCmd(0x40); //--set start line address

  OLED_Rotate(ROTATE_OFF);
  OLED_SetBrightness(0xFF);
  OLED_SetGlobalReverse(REVERSE_OFF);

  WriteCmd(0xA8); //--set multiplex ratio(1 to 64)
  WriteCmd(0x3F); //

  WriteCmd(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
  WriteCmd(0xD3); //-set display offset
  WriteCmd(0x00); //-not offset

  WriteCmd(0xD5); //--set display clock divide ratio/oscillator frequency
  WriteCmd(0xF0); //--set divide ratio

  WriteCmd(0xD9); //--set pre-charge period
  WriteCmd(0x22); //

  WriteCmd(0xDA); //--set com pins hardware configuration
  WriteCmd(0x12);

  WriteCmd(0xDB); //--set vcomh
  WriteCmd(0x20); //0x20,0.77xVcc

  WriteCmd(0x8D); //--set DC-DC enable
  WriteCmd(0x14); //
  WriteCmd(0xAF); //--turn on oled panel
  OLED_CLS();
}

void OLED_SetPos(uint8_t x, uint8_t y)
{
  WriteCmd(0xB0 + y);
  WriteCmd(((x & 0xF0) >> 4) | 0x10);
  WriteCmd((x & 0x0F) | 0x01);
}

void OLED_Fill(uint8_t data)
{
  uint16_t i;
  if (isScrolling)
    OLED_StopScroll();
  WriteCmd(0xB0);
  WriteCmd(0x00);
  WriteCmd(0x10);
  FastWrite_Start();
  for (i = 0; i < 1024; i++)
    FastWrite(data);
  FastWrite_Stop();
}

void OLED_CLS(void)
{
  OLED_Fill(0x00);
}

void OLED_ON(void)
{
  WriteCmd(0x8D);
  WriteCmd(0x14);
  WriteCmd(0xAF);
}

void OLED_OFF(void)
{
  WriteCmd(0x8D);
  WriteCmd(0x10);
  WriteCmd(0xAE);
}

uint8_t OLED_ShowStr(uint8_t x, uint8_t y, uint8_t *str)
{
  uint8_t c = 0, i = 0, j = 0, pt = 0;
  uint8_t currX = x, currY = y;
  if (isScrolling)
    OLED_StopScroll();
  if (textSize == TEXTSIZE_SMALL)
  {
    OLED_SetPos(currX, currY);
    FastWrite_Start();
    while (str[j] != '\0')
    {
      c = str[j] - ' ';
      if (currX > 122)
      {
        FastWrite_Stop();
        currX = 0;
        currY++;
        OLED_SetPos(currX, currY);
        FastWrite_Start();
      }
      for (i = 0; i < 6; i++)
        FastWrite(F6x8[c][i]);
      currX += 6;
      j++;
    }
    FastWrite_Stop();
  }
  else if (textSize == TEXTSIZE_BIG)
  {
    for (pt = 0; pt < 2; pt++)
    {
      currX = x;
      currY = y;
      j = 0;
      OLED_SetPos(currX, currY + pt);
      FastWrite_Start();
      while (str[j] != '\0')
      {
        c = str[j] - ' ';
        if (currX > 120)
        {
          FastWrite_Stop();
          currX = 0;
          currY++;
          OLED_SetPos(currX, currY + pt);
          FastWrite_Start();
        }
        for (i = 0; i < 8; i++)
          FastWrite(F8X16[c * 16 + i + 8 * pt]);
        currX += 8;
        j++;
      }
      FastWrite_Stop();
    }
  }
  if (textSize == TEXTSIZE_SMALL || textSize == TEXTSIZE_BIG)
  {
    OLED_cursorX = currX;
    OLED_cursorY = currY;
  }
  return j;
}

void OLED_ShowCN(uint8_t x, uint8_t y, uint8_t index)
{
  uint8_t i = 0;
  uint32_t adder = 32 * index;
  if (isScrolling)
    OLED_StopScroll();
  OLED_SetPos(x, y);
  FastWrite_Start();
  for (i = 0; i < 16; i++)
  {
    FastWrite(F16x16[adder]);
    adder += 1;
  }
  FastWrite_Stop();
  OLED_SetPos(x, y + 1);
  FastWrite_Start();
  for (i = 0; i < 16; i++)
  {
    FastWrite(F16x16[adder]);
    adder += 1;
  }
  FastWrite_Stop();
  OLED_cursorX = x + 16;
  OLED_cursorY = y;
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch)
{
  uint8_t i = 0;
  if (isScrolling)
    OLED_StopScroll();
  if (textSize == TEXTSIZE_SMALL)
  {
    ch -= ' ';
    OLED_SetPos(x, y);
    FastWrite_Start();
    for (i = 0; i < 6; i++)
      FastWrite(F6x8[ch][i]);
    FastWrite_Stop();
    OLED_cursorX = x + 6;
    OLED_cursorY = y;
  }
  else if (textSize == TEXTSIZE_BIG)
  {
    ch -= ' ';
    OLED_SetPos(x, y);
    FastWrite_Start();
    for (i = 0; i < 8; i++)
      FastWrite(F8X16[ch * 16 + i]);
    FastWrite_Stop();
    OLED_SetPos(x, y + 1);
    FastWrite_Start();
    for (i = 0; i < 8; i++)
      FastWrite(F8X16[ch * 16 + 8 + i]);
    FastWrite_Stop();
    OLED_cursorX = x + 8;
    OLED_cursorY = y;
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
  if (state)
    WriteCmd(0xA7);
  else
    WriteCmd(0xA6);
}

void OLED_SetCurrentReverse(uint8_t state)
{
  isCurrentReverse = !!state;
}

void OLED_SetTextSize(uint8_t size)
{
  textSize = size;
}

void OLED_Rotate(uint8_t state)
{
  if (state)
  {
    WriteCmd(0xC0);
    WriteCmd(0xA0);
  }
  else
  {
    WriteCmd(0xC8);
    WriteCmd(0xA1);
  }
}

void OLED_StopScroll(void)
{
  WriteCmd(0x2E);
  isScrolling = 0;
}

void OLED_Scroll(uint8_t direction, uint8_t startPage, uint8_t endPage, uint8_t hSpeed, uint8_t vSpeed)
{
  static const uint8_t hSpeedReg[8] = {0x3, 0x2, 0x1, 0x6, 0x0, 0x5, 0x4, 0x7}; // framePerT: 256, 128, 64, 25, 5, 4, 3, 2
  if (startPage > 7)
    startPage = 7;
  if (endPage > 7)
    endPage = 7;
  if (endPage < startPage)
    endPage = startPage;
  hSpeed &= 0x7;  // & 3'b111
  vSpeed &= 0x3F; // & 6'b111111

  OLED_StopScroll();
  WriteCmd(0x28 | (direction & 0x03));
  WriteCmd(0x00);
  WriteCmd(startPage);
  WriteCmd(hSpeedReg[hSpeed]);
  WriteCmd(endPage);
  WriteCmd(vSpeed);
  WriteCmd(0x2F); // Start Scroll
  isScrolling = 1;
}