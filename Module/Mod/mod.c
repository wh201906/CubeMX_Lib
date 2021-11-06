#include "mod.h"
#include "OLED/oled.h"
#include "tim.h"

uint8_t Mod_RxBuf[32];
uint8_t Mod_RxBufBegin, Mod_RxBufEnd;
uint32_t Mod_RxThre = 250;

uint8_t Mod_TxId;
uint32_t Mod_TxBuf[32];
uint16_t Mod_TxData;
uint32_t Mod_TxL, Mod_TxH;
TIM_TypeDef *Mod_TxOutTIM;
TIM_TypeDef *Mod_TxClkTIM;

void Mod_Tx_Init(TIM_TypeDef *outTIM, TIM_TypeDef *clkTIM, uint32_t Larr, uint32_t Harr)
{
  Mod_TxOutTIM = outTIM;
  Mod_TxClkTIM = clkTIM;
  Mod_TxL = Larr;
  Mod_TxH = Harr;
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
  LL_TIM_SetAutoReload(Mod_TxOutTIM, tmp);
  LL_TIM_OC_SetCompareCH1(Mod_TxOutTIM, (tmp + 1) / 2);
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
  else if(Mod_TxId == 0)
  {
    Mod_TxBuf[16] = Mod_TxL;
    for (i = 17; i < 31; i++)
    {
      Mod_TxBuf[i] = Mod_TxH;
    }
    Mod_TxBuf[31] = Mod_TxH;
  }
}

inline void Mod_Rx_Read(uint8_t bit)
{
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
    if (!(data & 0x80000000u) && (data & 0x1FFFFu) == 0x17FFFu) // pattern detected
    {
      data >>= 16 + 1;
      data &= 0x3FFF;
      printf("%u\n", data);
    }
    Mod_RxBufBegin++;
    Mod_RxBufBegin %= 32;
  }
}

void OLED_Show4digit(uint8_t x, uint8_t y, int64_t val)
{
  uint16_t i;
  uint8_t width = 8;
  for(i = 10; i <= 1000; i *= 10)
  {
    if(val < i)
    {
      OLED_ShowChar(x, y, '0');
      x += width;
    }
  }
  OLED_ShowInt(x, y, val);
}