#include "hmi.h"

#include "adc.h"
#include "dma.h"
#include "tim.h"

#include "DELAY/delay.h"

uint8_t HMI_CurrentPage = 'm';
uint8_t HMI_Buf[100];

uint8_t HMI_CurrentChannel = 0;

uint8_t HMI_THD_harmony = 5;
uint8_t HMI_THD_Counter = 0;
uint8_t HMI_THD_AutoMode = 0;

uint16_t HMI_Spectrum_windowType = 0;
uint16_t HMI_Spectrum_offsetX = 0;
uint16_t HMI_Spectrum_rangeX = 400;
uint16_t HMI_Spectrum_offsetY = 0;
uint16_t HMI_Spectrum_rangeY = 250;

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
    HMI_THDInit();
  else if (HMI_CurrentPage == 'w')
    HMI_WaveInit();
  else if (HMI_CurrentPage == 's')
    HMI_SpectrumInit();
}

void HMI_THDInit()
{
  __HAL_TIM_SET_AUTORELOAD(&htim2, 585);
  __HAL_TIM_SET_PRESCALER(&htim2, 0);
  htim2.Instance->EGR = TIM_EGR_UG;
  HAL_TIM_Base_Start(&htim2);
  MyFFT_Init(1);
  MyFFT_FlattopWindow();
  HMI_THD_UpdateHarmony();
  HMI_THD_UpdateSelection();
  HMI_THD_UpdateLabel();
}
void HMI_WaveInit()
{
}
void HMI_SpectrumInit()
{
  __HAL_TIM_SET_AUTORELOAD(&htim2, 585);
  __HAL_TIM_SET_PRESCALER(&htim2, 0);
  htim2.Instance->EGR = TIM_EGR_UG;
  HAL_TIM_Base_Start(&htim2);
  MyFFT_Init(1);
  HMI_Spectrum_SetWindow();
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
  if (HMI_THD_AutoMode)
  {
    HMI_THD_Counter++;
    HMI_THD_Counter %= 10; // 200ms*10
    if (HMI_THD_Counter == 0)
    {
      // Change channel
    }
  }
  Delay_ms(200);
}

void HMI_WavePage()
{
}

void HMI_SpectrumPage()
{
  uint8_t rxBuf[10];
  uint8_t displayBuf[400];
  double THD;
  int32_t i;
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)val, FFT_LENGTH);
  while (!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_OVR))
    ;
  hadc1.Instance->CR2 &= ~ADC_CR2_DMA;
  for (i = 0; i < FFT_LENGTH; i++)
    fftData[i] = val[i];
  MyFFT_CalcInPlace(fftData);

  HMI_Scale(fftData, displayBuf, HMI_Spectrum_offsetX, HMI_Spectrum_rangeX, HMI_Spectrum_offsetY, HMI_Spectrum_rangeY);
  MyUART_WriteStr(&uartHandle2, "ref_stop\xFF\xFF\xFF");
  MyUART_WriteStr(&uartHandle2, "cle 1,0\xFF\xFF\xFF");
  MyUART_WriteStr(&uartHandle2, "addt 1,0,400\xFF\xFF\xFF");

  // Wait for response in 30ms
  rxBuf[0] = 0x00;
  for (i = 0; i < 15; i++)
  {
    if (rxBuf[0] == 0xFE)
      break;
    Delay_ms(2);
    MyUART_Read(&uartHandle2, rxBuf, 4);
  }

  for (i = 0; i < 400; i++)
    MyUART_WriteChar(&uartHandle2, displayBuf[i]);
  Delay_ms(5);
  MyUART_WriteStr(&uartHandle2, "ref_star\xFF\xFF\xFF");
  Delay_ms(200);
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
      HMI_THD_UpdateHarmony();
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
  if (HMI_Buf[1] >= '0' && HMI_Buf[1] <= '3')
  {
    HMI_Spectrum_windowType = HMI_Buf[1] - '0';
    HMI_Spectrum_SetWindow();
  }
}

void HMI_THD_SetChannel(uint8_t channel)
{
  if (channel >= 0 && channel <= 4)
  {
    HMI_THD_AutoMode = 0;
    HMI_CurrentChannel = channel;
    // Change channel
  }
  else if (channel == 5)
  {
    HMI_THD_AutoMode = 1;
  }
}

// Scale from xLen*yLen to xRange*yRange
// xRange set to 400
// yRange set to 250
void HMI_Scale(float32_t *src, uint8_t *dst, uint32_t xBegin, uint32_t xLen, uint32_t yBegin, uint32_t yLen)
{
  uint16_t xRange = 400;
  uint16_t yRange = 250;
  double xRate, yRate, sI;
  uint32_t dI, sIL, sIH;
  xRate = (double)xLen / xRange;
  yRate = (double)yLen / yRange;
  src += xBegin;
  for (dI = 0; dI < xRange; dI++)
  {
    sI = (dI + 0.5) * xRate - 0.5;
    sIL = sI;
    sIH = sI + 1;
    if (sIL < 0)
      sIL = 0;
    else if (sIH >= xLen)
      sIH = xLen - 1;
    dst[dI] = ((src[sIH] - src[sIL]) * (sI - sIL) + src[sIL]) * yRate - (yBegin * yRate) + 0.5;
  }
}

// gb2312
void HMI_THD_UpdateLabel()
{
  if (HMI_CurrentChannel == 0)
    MyUART_WriteStr(&uartHandle2, "currtype.txt=\"��ǰ���Σ���������\"\xFF\xFF\xFF");
  else if (HMI_CurrentChannel == 1)
    MyUART_WriteStr(&uartHandle2, "currtype.txt=\"��ǰ���Σ�����ʧ��\"\xFF\xFF\xFF");
  else if (HMI_CurrentChannel == 2)
    MyUART_WriteStr(&uartHandle2, "currtype.txt=\"��ǰ���Σ��ײ�ʧ��\"\xFF\xFF\xFF");
  else if (HMI_CurrentChannel == 3)
    MyUART_WriteStr(&uartHandle2, "currtype.txt=\"��ǰ���Σ�˫��ʧ��\"\xFF\xFF\xFF");
  else if (HMI_CurrentChannel == 4)
    MyUART_WriteStr(&uartHandle2, "currtype.txt=\"��ǰ���Σ���Խʧ��\"\xFF\xFF\xFF");
}

void HMI_THD_UpdateHarmony()
{
  uint8_t tmp[20];
  myitoa(HMI_THD_harmony, tmp, 10);
  MyUART_WriteStr(&uartHandle2, "harmony.val=");
  MyUART_WriteStr(&uartHandle2, tmp);
  MyUART_WriteStr(&uartHandle2, "\xFF\xFF\xFF");
}

void HMI_THD_UpdateSelection()
{
  MyUART_WriteStr(&uartHandle2, "r0.val=0\xFF\xFF\xFF");
  MyUART_WriteStr(&uartHandle2, "r1.val=0\xFF\xFF\xFF");
  MyUART_WriteStr(&uartHandle2, "r2.val=0\xFF\xFF\xFF");
  MyUART_WriteStr(&uartHandle2, "r3.val=0\xFF\xFF\xFF");
  MyUART_WriteStr(&uartHandle2, "r4.val=0\xFF\xFF\xFF");
  MyUART_WriteStr(&uartHandle2, "r5.val=0\xFF\xFF\xFF");
  if (HMI_THD_AutoMode)
    MyUART_WriteStr(&uartHandle2, "r5.val=1\xFF\xFF\xFF");
  else
  {
    MyUART_WriteStr(&uartHandle2, "r");
    MyUART_WriteChar(&uartHandle2, '0' + HMI_CurrentChannel);
    MyUART_WriteStr(&uartHandle2, ".val=1\xFF\xFF\xFF");
  }
}

void HMI_Spectrum_UpdateWindow()
{
  MyUART_WriteStr(&uartHandle2, "r0.val=0\xFF\xFF\xFF");
  MyUART_WriteStr(&uartHandle2, "r1.val=0\xFF\xFF\xFF");
  MyUART_WriteStr(&uartHandle2, "r2.val=0\xFF\xFF\xFF");
  MyUART_WriteStr(&uartHandle2, "r3.val=0\xFF\xFF\xFF");
  MyUART_WriteStr(&uartHandle2, "r");
  MyUART_WriteChar(&uartHandle2, '0' + HMI_Spectrum_windowType);
  MyUART_WriteStr(&uartHandle2, ".val=1\xFF\xFF\xFF");
}

void HMI_Spectrum_SetWindow()
{
  if (HMI_Spectrum_windowType == 0)
    MyFFT_NoWindow();
  else if (HMI_Spectrum_windowType == 1)
    MyFFT_HannWindow();
  else if (HMI_Spectrum_windowType == 2)
    MyFFT_HammingWindow();
  else if (HMI_Spectrum_windowType == 3)
    MyFFT_FlattopWindow();
}