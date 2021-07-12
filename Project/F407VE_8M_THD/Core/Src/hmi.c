#include "hmi.h"

#include "adc.h"
#include "dma.h"
#include "tim.h"

uint8_t HMI_CurrentPage = 'm';
uint8_t HMI_Buf[100];

uint8_t HMI_THD_harmony = 5;
uint8_t HMI_THD_Counter = 0;
uint8_t HMI_CurrentChannel = 0;
uint8_t HMI_AutoMode = 0;

extern MyUARTHandle uartHandle1, uartHandle2;
extern uint16_t val[FFT_LENGTH];
extern float32_t fftData[FFT_LENGTH];

void HMI_Process()
{
  uint32_t tmp;
  if (HMI_CurrentPage == 't')
    HMI_THDPage();
  else if (HMI_CurrentPage == 's')
    HMI_SpectrumPage();
  else if (HMI_CurrentPage == 'w')
    HMI_WavePage();

  // Instruction process
  tmp = MyUART_ReadUntil(&uartHandle2, HMI_Buf, '>');
  if (!tmp)
    return;
  HMI_Buf[tmp] = '>';
  if (HMI_Buf[0] == 'p')
  {
    HMI_CurrentPage = HMI_Buf[1];
    HMI_PageInit();
  }
  else if (HMI_Buf[0] != HMI_CurrentPage)
  {
    HMI_CurrentPage = HMI_Buf[0];
    HMI_PageInit();
  }
  if (HMI_Buf[0] == 't')
    HMI_THDInst();
  else if (HMI_Buf[0] == 's')
    HMI_SpectrumInst();
  else if (HMI_Buf[0] == 'w')
    HMI_WaveInst();
}

void HMI_PageInit()
{
  if (HMI_CurrentPage == 't')
  {
    __HAL_TIM_SET_AUTORELOAD(&htim2, 585);
    __HAL_TIM_SET_PRESCALER(&htim2, 0);
    htim2.Instance->EGR = TIM_EGR_UG;
    HAL_TIM_Base_Start(&htim2);
    MyFFT_Init(1);
    MyFFT_FlattopWindow();
    MyUART_WriteStr(&uartHandle2, "r0.val=0\xFF\xFF\xFF");
    MyUART_WriteStr(&uartHandle2, "r1.val=0\xFF\xFF\xFF");
    MyUART_WriteStr(&uartHandle2, "r2.val=0\xFF\xFF\xFF");
    MyUART_WriteStr(&uartHandle2, "r3.val=0\xFF\xFF\xFF");
    MyUART_WriteStr(&uartHandle2, "r4.val=0\xFF\xFF\xFF");
    MyUART_WriteStr(&uartHandle2, "r5.val=0\xFF\xFF\xFF");
    if (HMI_AutoMode)
      MyUART_WriteStr(&uartHandle2, "r5.val=1\xFF\xFF\xFF");
    else
    {
      MyUART_WriteStr(&uartHandle2, "r");
      MyUART_WriteChar(&uartHandle2, '0' + HMI_CurrentChannel);
      MyUART_WriteStr(&uartHandle2, ".val=1\xFF\xFF\xFF");
      HMI_THD_UpdateLabel();
    }
  }
  else if (HMI_CurrentPage == 's')
  {
    __HAL_TIM_SET_AUTORELOAD(&htim2, 585);
    __HAL_TIM_SET_PRESCALER(&htim2, 0);
    htim2.Instance->EGR = TIM_EGR_UG;
    HAL_TIM_Base_Start(&htim2);
    MyFFT_Init(1);
  }
}

void HMI_THDPage()
{
  double THD;
  int32_t i;
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)val, FFT_LENGTH);
  while (!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_OVR))
    ;
  hadc1.Instance->CR2 &= ~ADC_CR2_DMA;
  for (i = 0; i < FFT_LENGTH; i++)
    fftData[i] = val[i];
  MyFFT_CalcInPlace(fftData);
  THD = MyFFT_THD(fftData, FFT_LENGTH / 2, 5, HMI_THD_harmony);
  myftoa(THD * 100, HMI_Buf);
  MyUART_WriteStr(&uartHandle2, "thdtext.txt=\"");
  MyUART_WriteStr(&uartHandle2, HMI_Buf);
  MyUART_WriteStr(&uartHandle2, "%\"\xFF\xFF\xFF");
  Delay_ms(200);
  if (HMI_AutoMode)
  {
    HMI_THD_Counter++;
    HMI_THD_Counter %= 10; // 200ms*10
    if (HMI_THD_Counter == 0)
    {
      // Change channel
    }
  }
}

void HMI_WavePage()
{
}

void HMI_SpectrumPage()
{
}

void HMI_THDInst()
{
  uint8_t tmp;
  if (HMI_Buf[1] == 'h')
  {
    tmp = myatoi(HMI_Buf + 2);
    if ((tmp == HMI_THD_harmony + 1 || tmp == HMI_THD_harmony - 1) && tmp != 0)
    {
      HMI_THD_harmony = tmp;
    }
    else
    {
      myitoa(HMI_THD_harmony, HMI_Buf, 10);
      MyUART_WriteStr(&uartHandle2, "harmony.val=");
      MyUART_WriteStr(&uartHandle2, HMI_Buf);
      MyUART_WriteStr(&uartHandle2, "\xFF\xFF\xFF");
    }
  }
  else if (HMI_Buf[1] >= '0' && HMI_Buf[1] <= '5')
  {
    HMI_THD_SetChannel(HMI_Buf[1] - '0');
  }
}

void HMI_WaveInst()
{
}

void HMI_SpectrumInst()
{
}

void HMI_THD_SetChannel(uint8_t channel)
{
  if (channel >= 0 && channel <= 4)
  {
    HMI_AutoMode = 0;
    HMI_CurrentChannel = channel;
    // Change channel
  }
  else if (channel == 5)
  {
    HMI_AutoMode = 1;
  }
}

void HMI_Scale(uint8_t *src, uint32_t srcBegin, uint32_t srcLen, uint8_t *dst, uint32_t dstBegin, uint32_t dstLen)
{
  double r, sI;
  uint32_t dI, sIL, sIH;
  r = (double)srcLen / dstLen;
  src += srcBegin;
  dst += dstBegin;
  for (dI = 0; dI < dstLen; dI++)
  {
    sI = (dI + 0.5) * r - 0.5;
    sIL = sI;
    sIH = sI + 1;
    if (sIL < 0)
      sIL = 0;
    else if (sIH >= srcLen)
      sIH = srcLen - 1;
    dst[dI] = (src[sIH] - src[sIL]) * (sI - sIL) + src[sIL] + 0.5;
  }
}

// gb2312
void HMI_THD_UpdateLabel()
{
  if (HMI_CurrentChannel == 0)
    MyUART_WriteStr(&uartHandle2, "currtype.txt=\"当前波形：正常波形\"\xFF\xFF\xFF");
  else if (HMI_CurrentChannel == 1)
    MyUART_WriteStr(&uartHandle2, "currtype.txt=\"当前波形：顶部失真\"\xFF\xFF\xFF");
  else if (HMI_CurrentChannel == 2)
    MyUART_WriteStr(&uartHandle2, "currtype.txt=\"当前波形：底部失真\"\xFF\xFF\xFF");
  else if (HMI_CurrentChannel == 3)
    MyUART_WriteStr(&uartHandle2, "currtype.txt=\"当前波形：双向失真\"\xFF\xFF\xFF");
  else if (HMI_CurrentChannel == 4)
    MyUART_WriteStr(&uartHandle2, "currtype.txt=\"当前波形：交越失真\"\xFF\xFF\xFF");
}