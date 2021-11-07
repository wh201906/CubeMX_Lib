#include "mod.h"
#include "OLED/oled.h"
#include "GRIDKEY/gridkey.h"
#include "AD9834/AD9834.h"
#include "tim.h"

#define MOD_FREQLIST_LEN 11
#define MOD_NUM_OFFSET 3

uint8_t Mod_RxBuf[32];
uint8_t Mod_RxBufBegin, Mod_RxBufEnd;
uint32_t Mod_RxThre = 250;
uint8_t Mod_RxDigit[4] = {16, 16, 16, 16};

uint8_t Mod_TxId; // next bit
uint32_t Mod_TxBuf[32];
uint16_t Mod_TxData = 0xFFFF;
uint32_t Mod_TxL, Mod_TxH;
TIM_TypeDef *Mod_TxOutTIM;
TIM_TypeDef *Mod_TxClkTIM;

uint16_t Mod_TxCurrNum = 1234;
uint8_t Mod_TxIsSending = 0;
uint16_t Mod_TxInput;
uint16_t Mod_TxEditNum = 0;
uint8_t Mod_TxEditState = 5;
uint32_t Mod_freqList[MOD_FREQLIST_LEN] =
    {29.00e6, 29.05e6, 29.10e6, 29.15e6,
     29.20e6, 29.25e6, 29.30e6, 29.35e6,
     29.40e6, 29.45e6, 29.50e6};
uint8_t Mod_freqId = 0; // current freq
     
// 0~3: editing 4: finished 5: not editing
// ------------------
// 0    1    2    3
// 4    5    6    7
// 8    9    Ok   Cancel
// Send Freq x    Stop
// ------------------
// ------------------
// 0  1  2  3
// 4  5  6  7
// 8  9  10 11
// 12 13 14 15
// ------------------
#ifndef MOD_RX
void Mod_Tx_Init(TIM_TypeDef *outTIM, TIM_TypeDef *clkTIM, uint32_t Larr, uint32_t Harr)
{
  Mod_TxOutTIM = outTIM;
  Mod_TxClkTIM = clkTIM;
  Mod_TxL = Larr;
  Mod_TxH = Harr;
  
  OLED_Init(GPIOB, 9, GPIOB, 8);
  OLED_SetTextSize(TEXTSIZE_BIG);
  AD9834_Init(&hspi1);
  Mod_freqId = 0;
  AD9834_SetFreq(Mod_freqList[Mod_freqId], 0);
  OLED_ShowStr(0, 0, "Stopped");
  OLED_ShowStr(0, 2, "1234 ");
  OLED_ShowStr(0, 4, "     kHz");
  OLED_ShowInt(0, 4, Mod_freqList[Mod_freqId] / 1000);
}

void Mod_Tx_Start(void)
{
  uint8_t i;

  Mod_TxId = 0;
  for (i = 0; i < 32; i++)
    Mod_TxBuf[i] = Mod_TxH;
  LL_TIM_SetAutoReload(Mod_TxOutTIM, Mod_TxH);
  LL_TIM_OC_SetCompareCH1(Mod_TxOutTIM, (Mod_TxH + 1) / 2);
  LL_TIM_SetCounter(Mod_TxOutTIM, 0);
  LL_TIM_SetCounter(Mod_TxClkTIM, 0);
  Mod_TxOutTIM->EGR = TIM_EGR_UG; // force update
  Mod_TxClkTIM->EGR = TIM_EGR_UG; // force update
  LL_TIM_CC_EnableChannel(Mod_TxOutTIM, LL_TIM_CHANNEL_CH1);
  LL_TIM_EnableIT_UPDATE(Mod_TxClkTIM);
  LL_TIM_EnableCounter(Mod_TxOutTIM);
  LL_TIM_EnableCounter(Mod_TxClkTIM);
}

void Mod_Tx_SetValue(uint16_t data)
{
  Mod_TxData = data;
  LL_TIM_SetCounter(Mod_TxClkTIM, 0);
  Mod_TxId = 0;
}

inline void Mod_Tx_Send(void)
{
  uint8_t i;
  uint32_t tmp;

  tmp = Mod_TxBuf[Mod_TxId];
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, tmp == Mod_TxH); // unmodulated
  LL_TIM_SetAutoReload(Mod_TxOutTIM, tmp);              // modulated
  LL_TIM_OC_SetCompareCH1(Mod_TxOutTIM, (tmp + 1) / 2); // modulated
  Mod_TxId++;
  Mod_TxId %= 32;
  if (Mod_TxId == 16)
  {
    Mod_TxBuf[0] = Mod_TxL;
    for (i = 1; i < 15; i++)
    {
      Mod_TxBuf[i] = (Mod_TxData << (i - 1) & 0x2000) ? Mod_TxH : Mod_TxL;
    }
    Mod_TxBuf[15] = Mod_TxH;
  }
  else if (Mod_TxId == 0)
  {
    Mod_TxBuf[16] = Mod_TxL;
    for (i = 17; i < 31; i++)
    {
      Mod_TxBuf[i] = Mod_TxH;
    }
    Mod_TxBuf[31] = Mod_TxH;
  }
}

void Mod_Tx_Process(void)
{
  Mod_TxInput = GridKey_Scan(2);
  if (Mod_TxInput != 0xFF)
  {
    if (Mod_TxEditState == 5) // not editing
    {
      if (Mod_TxInput == 10) // ok, start editing
      {
        Mod_TxEditNum = 0;
        OLED_ShowStr(0, 2, ">    ");
        Mod_TxEditState = 0;
      }
    }
    else // editing
    {
      if (Mod_TxInput == 11) // cancel
      {
        OLED_ShowStr(0, 2, "     ");
        OLED_Show4digit(0, 2, Mod_TxCurrNum);
        Mod_TxEditState = 5;
      }
      else if (Mod_TxInput == 10) // ok, confirm
      {
        Mod_TxCurrNum = Mod_TxEditNum;
        if (Mod_TxIsSending)
          Mod_Tx_SetValue(Mod_TxCurrNum + MOD_NUM_OFFSET);
        OLED_ShowStr(0, 2, "     ");
        OLED_Show4digit(0, 2, Mod_TxCurrNum);
        Mod_TxEditState = 5;
      }
      else if (Mod_TxInput < 10 && Mod_TxEditState < 4)
      {
        Mod_TxEditNum *= 10;
        Mod_TxEditNum += Mod_TxInput;
        Mod_TxEditState++;
        OLED_ShowStr(0, 2, ">    ");
        OLED_Show4digit(8, 2, Mod_TxEditNum);
      }
    }
    if (Mod_TxInput == 12) // send
    {
      Mod_TxIsSending = 1;
      Mod_Tx_SetValue(Mod_TxCurrNum + MOD_NUM_OFFSET);
      OLED_ShowStr(0, 0, "Sending     \x81\x81");
    }
    else if (Mod_TxInput == 15) // stop
    {
      Mod_TxIsSending = 0;
      Mod_Tx_SetValue(0xFFFF);
      OLED_ShowStr(0, 0, "Stopped       ");
    }
    else if (Mod_TxInput == 13)
    {
      Mod_freqId++;
      Mod_freqId %= MOD_FREQLIST_LEN;
      AD9834_SetFreq(Mod_freqList[Mod_freqId], 0);
      OLED_ShowStr(0, 4, "     kHz");
      OLED_ShowInt(0, 4, Mod_freqList[Mod_freqId] / 1000);
    }
  }
}

void OLED_Show4digit(uint8_t x, uint8_t y, int64_t val)
{
  uint16_t i;
  uint8_t width = 8;
  for (i = 10; i <= 1000; i *= 10)
  {
    if (val < i)
    {
      OLED_ShowChar(x, y, '0');
      x += width;
    }
  }
  OLED_ShowInt(x, y, val);
}

#else
inline void Mod_Rx_Read(uint8_t bit)
{
  uint8_t valid = 0, reversedValid = 0;
  uint32_t i;
  uint32_t data;
  Mod_RxBuf[Mod_RxBufEnd] = bit;
  Mod_RxBufEnd++;
  Mod_RxBufEnd %= 32;
  if (Mod_RxBufBegin == Mod_RxBufEnd) // full
  {
    data = 0;
    for (i = Mod_RxBufBegin; (i + 1) % 32 != Mod_RxBufEnd; i++)
    {
      data <<= 1;
      data |= Mod_RxBuf[i % 32];
    }
    data <<= 1;
    data |= Mod_RxBuf[i % 32];
    // valid = (!(data & 0x80000000u) && (data & 0x1FFFFu) == 0x17FFFu); // pattern detected
    // reversedValid = ((data & 0x80000000u) && (data & 0x1FFFFu) == 0x17FFFu); // reversed pattern detected
    if (!(data & 0x80000000u) && (data & 0x1FFFFu) == 0x17FFFu) // pattern detected
    {
      data >>= 16 + 1;
      data &= 0x3FFF;
      if(data == 0x3FFF) // off
      {
        Mod_RxDigit[0] = 16;
        Mod_RxDigit[1] = 16;
        Mod_RxDigit[2] = 16;
        Mod_RxDigit[3] = 16;
      }
      else
      {
        data -= MOD_NUM_OFFSET;
        Mod_RxDigit[0] = data / 1000;
        data %= 1000;
        Mod_RxDigit[1] = data / 100;
        data %= 100;
        Mod_RxDigit[2] = data / 10;
        data %= 10;
        Mod_RxDigit[3] = data;
      }
      TM1637_SetNum(Mod_RxDigit);
    }
    Mod_RxBufBegin++;
    Mod_RxBufBegin %= 32;
  }
}
#endif