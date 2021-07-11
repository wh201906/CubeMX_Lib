#include "hmi.h"

#include "adc.h"
#include "dma.h"
#include "tim.h"

uint8_t HMI_CurrentPage = 'm';
uint8_t HMI_Buf[100];

uint8_t HMI_THD_harmony = 5;

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
    return;
  }
}

void HMI_THDPage()
{
  double THD;
  int32_t i;
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)val, FFT_LENGTH);
  while (!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_OVR))
    ;
  for (i = 0; i < FFT_LENGTH; i++)
    fftData[i] = val[i];
  MyFFT_CalcInPlace(fftData);
  THD = MyFFT_THD(fftData, FFT_LENGTH / 2, 5, HMI_THD_harmony);
  myftoa(THD * 100, HMI_Buf);
  MyUART_WriteStr(&uartHandle2, "thdtext.txt=\"");
  MyUART_WriteStr(&uartHandle2, HMI_Buf);
  MyUART_WriteStr(&uartHandle2, "%\"\xFF\xFF\xFF");
  Delay_ms(200);
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
}

void HMI_WaveInst()
{
}

void HMI_SpectrumInst()
{
}