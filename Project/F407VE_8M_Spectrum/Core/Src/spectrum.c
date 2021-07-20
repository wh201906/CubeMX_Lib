#include "spectrum.h"

#include "DELAY/delay.h"
#include "ADF4351/adf4351.h"
#include "UTIL/util.h"
#include "tim.h"

#define SPECTRUM_PAGE_MAIN '0'
#define SPECTRUM_PAGE_SPECTRUM '1'
#define SPECTRUM_PAGE_SWEEP '2'

extern MyUARTHandle uart2;

uint8_t Spectrum_CurrentPage = SPECTRUM_PAGE_MAIN;
uint8_t Spectrum_Buf[100];

ADF4351_CLKConfig PLLConfig;

uint16_t Spectrum_Sweep_amp;
double Spectrum_Sweep_begin;
double Spectrum_Sweep_end;
uint32_t Spectrum_Sweep_delay; // in us
double Spectrum_Sweep_step;
double Spectrum_Sweep_freq;

void Spectrum_Process()
{
  if (Spectrum_CurrentPage == SPECTRUM_PAGE_SPECTRUM)
    //Spectrum_SpectrumPage();
    ;
  else if (Spectrum_CurrentPage == SPECTRUM_PAGE_SWEEP)
    Spectrum_SweepPage();

  // Instruction process
  if (!MyUART_ReadUntilWithEnd(&uart2, Spectrum_Buf, '>'))
  {
    if (MyUART_IsFull(&uart2))
      MyUART_ClearBuffer(&uart2);
    return;
  }
  if (Spectrum_Buf[0] != 'p' && Spectrum_Buf[0] != SPECTRUM_PAGE_MAIN && Spectrum_Buf[0] != SPECTRUM_PAGE_SPECTRUM && Spectrum_Buf[0] != SPECTRUM_PAGE_SWEEP)
    return;
  if (Spectrum_Buf[0] == 'p')
  {
    Spectrum_SendCMD("ref_star");
    Spectrum_CurrentPage = Spectrum_Buf[1];
    Spectrum_PageInit();
  }
  else if (Spectrum_Buf[0] != Spectrum_CurrentPage)
  {
    Spectrum_SendCMD("ref_star");
    Spectrum_CurrentPage = Spectrum_Buf[0];
    Spectrum_PageInit();
  }
  if (Spectrum_Buf[0] == SPECTRUM_PAGE_SPECTRUM)
    ; //Spectrum_SpectrumInst();
  else if (Spectrum_Buf[0] == SPECTRUM_PAGE_SWEEP)
    Spectrum_SweepInst();
}

void Spectrum_PageInit()
{
  if (Spectrum_CurrentPage == SPECTRUM_PAGE_SPECTRUM)
    ; //Spectrum_SpectrumInit();
  else if (Spectrum_CurrentPage == SPECTRUM_PAGE_SWEEP)
    Spectrum_SweepInit();
}

void Spectrum_SweepInit()
{
  __HAL_TIM_SET_PRESCALER(&htim2, 84 - 1);
  htim2.Instance->EGR = TIM_EGR_UG;
  ADF4351_Init();
  ADF4351_SetCLKConfig(&PLLConfig, 100, 25, 0, 1, 32, 0.001);
  ADF4351_WriteCLKConfig(&PLLConfig);
}

// void Spectrum_SpectrumInit()
// {
// }

void Spectrum_SweepPage()
{
  Spectrum_Sweep_UpdateFreq();
  Delay_ms(100);
}

// void Spectrum_WavePage()
// {
//   int32_t i;
//   uint8_t displayBuf[400];

//   HAL_ADC_Start_DMA(&hadc1, (uint32_t *)val, FFT_LENGTH);
//   while (!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_OVR))
//     ;
//   hadc1.Instance->CR2 &= ~ADC_CR2_DMA;

//   for (i = 0; i < 400; i++)
//     displayBuf[i] = val[i] >> 4;

//   Spectrum_SendCMD("ref_stop");
//   Spectrum_SendCMD("cle 1,0");
//   Spectrum_SendCMD("addt 1,0,400");
//   Spectrum_WaitResponse(0xFE, 30);

//   for (i = 0; i < 400; i++)
//     MyUART_WriteChar(&uart2, displayBuf[i]);
//   Spectrum_WaitResponse(0xFD, 30);

//   Spectrum_SendCMD("ref_star");
//   Delay_ms(200);
// }

// void Spectrum_SpectrumPage()
// {
//   uint8_t rxBuf[10];
//   uint8_t displayBuf[400];
//   int32_t i;

//   Spectrum_DoFFT(0);
//   Spectrum_Scale(fftData, displayBuf, Spectrum_Spectrum_offsetX, Spectrum_Spectrum_rangeX, Spectrum_Spectrum_offsetY, Spectrum_Spectrum_rangeY);
//   Spectrum_SendCMD("ref_stop");
//   Spectrum_SendCMD("cle 1,0");
//   Spectrum_SendCMD("addt 1,0,400");
//   Spectrum_WaitResponse(0xFE, 30);

//   for (i = 0; i < 400; i++)
//     MyUART_WriteChar(&uart2, displayBuf[i]);
//   Spectrum_WaitResponse(0xFD, 30);

//   Spectrum_SendCMD("ref_star");
//   Delay_ms(200);
// }

// void Spectrum_SpectrumInst()
// {
// }

void Spectrum_SweepInst()
{
  uint32_t tmp;
  if (Spectrum_Buf[1] == 'a')
  {
    tmp = myatoi(Spectrum_Buf + 2);
    Spectrum_Sweep_amp = tmp;
    if (tmp == 0)
      ADF4351_SetOutputPower(ADF4351_R4_PWR_N4DBM);
    else if (tmp == 1)
      ADF4351_SetOutputPower(ADF4351_R4_PWR_N1DBM);
    else if (tmp == 2)
      ADF4351_SetOutputPower(ADF4351_R4_PWR_2DBM);
    else if (tmp == 3)
      ADF4351_SetOutputPower(ADF4351_R4_PWR_5DBM);
  }
  else if (Spectrum_Buf[1] == 'b')
  {
    tmp = myatoi(Spectrum_Buf + 2);
    Spectrum_Sweep_begin = tmp / 1000.0;
  }
  else if (Spectrum_Buf[1] == 'e')
  {
    tmp = myatoi(Spectrum_Buf + 2);
    Spectrum_Sweep_end = tmp / 1000.0;
  }
  else if (Spectrum_Buf[1] == 's')
  {
    tmp = myatoi(Spectrum_Buf + 2);
    Spectrum_Sweep_step = tmp / 1000.0;
  }
  else if (Spectrum_Buf[1] == 't')
  {
    tmp = myatoi(Spectrum_Buf + 2);
    Spectrum_Sweep_delay = tmp * 1000.0 * Spectrum_Sweep_step / (Spectrum_Sweep_end - Spectrum_Sweep_begin);
  }
  else if (Spectrum_Buf[1] == 'd')
  {
    tmp = myatoi(Spectrum_Buf + 2);
    Spectrum_Sweep_freq = tmp / 1000.0;
    ADF4351_SetCLKConfig(&PLLConfig, 100, 25, 0, 1, ADF4351_CalcDiv(Spectrum_Sweep_freq), 0.001);
    ADF4351_WriteCLKConfig(&PLLConfig);
    ADF4351_SetFreq(&PLLConfig, Spectrum_Sweep_freq);
    Spectrum_Sweep_UpdateFreq();
  }
  else if (Spectrum_Buf[1] == '0')
  {
    Spectrum_Sweep_freq = Spectrum_Sweep_begin;
    ADF4351_SetCLKConfig(&PLLConfig, 100, 25, 0, 1, 32, 0.001);
    ADF4351_WriteCLKConfig(&PLLConfig);
    __HAL_TIM_SET_AUTORELOAD(&htim2, Spectrum_Sweep_delay - 1);
    htim2.Instance->EGR = TIM_EGR_UG;
    HAL_TIM_Base_Start_IT(&htim2);
  }
  else if (Spectrum_Buf[1] == '1')
  {
    HAL_TIM_Base_Stop_IT(&htim2);
  }
}

// void Spectrum_SpectrumInst()
// {
//   uint16_t tmp;
//   if (Spectrum_Buf[1] >= '0' && Spectrum_Buf[1] <= '3')
//   {
//     Spectrum_Spectrum_windowType = Spectrum_Buf[1] - '0';
//     Spectrum_Spectrum_SetWindow();
//   }
//   else if (Spectrum_Buf[1] == 'x')
//   {
//     if (Spectrum_Buf[2] == 's')
//     {
//       tmp = myatoi(Spectrum_Buf + 3);
//       if (tmp < FFT_LENGTH / 2)
//         Spectrum_Spectrum_offsetX = tmp;
//     }
//     else if (Spectrum_Buf[2] == 'u')
//     {
//       if (Spectrum_Spectrum_rangeX == 100)
//         Spectrum_Spectrum_rangeX = 200;
//       else if (Spectrum_Spectrum_rangeX == 200)
//         Spectrum_Spectrum_rangeX = 400;
//       else if (Spectrum_Spectrum_rangeX == 400)
//         Spectrum_Spectrum_rangeX = 1000;
//       else if (Spectrum_Spectrum_rangeX == 1000)
//         Spectrum_Spectrum_rangeX = 2000;
//       myitoa(FFT_LENGTH / 2 - Spectrum_Spectrum_rangeX, Spectrum_Buf, 10);
//       MyUART_WriteStr(&uart2, "sliderX.maxval=");
//       MyUART_WriteStr(&uart2, Spectrum_Buf);
//       MyUART_WriteStr(&uart2, "\xFF\xFF\xFF");
//     }
//     else if (Spectrum_Buf[2] == 'd')
//     {
//       if (Spectrum_Spectrum_rangeX == 200)
//         Spectrum_Spectrum_rangeX = 100;
//       else if (Spectrum_Spectrum_rangeX == 400)
//         Spectrum_Spectrum_rangeX = 200;
//       else if (Spectrum_Spectrum_rangeX == 1000)
//         Spectrum_Spectrum_rangeX = 400;
//       else if (Spectrum_Spectrum_rangeX == 2000)
//         Spectrum_Spectrum_rangeX = 1000;
//       myitoa(FFT_LENGTH / 2 - Spectrum_Spectrum_rangeX, Spectrum_Buf, 10);
//       MyUART_WriteStr(&uart2, "sliderX.maxval=");
//       MyUART_WriteStr(&uart2, Spectrum_Buf);
//       MyUART_WriteStr(&uart2, "\xFF\xFF\xFF");
//     }
//   }
//   else if (Spectrum_Buf[1] == 'y')
//   {
//     if (Spectrum_Buf[2] == 's')
//     {
//       tmp = myatoi(Spectrum_Buf + 3);
//       if (tmp < FFT_LENGTH / 2)
//         Spectrum_Spectrum_offsetY = tmp;
//     }
//   }
// }

// void Spectrum_THD_SetChannel(uint8_t channel)
// {
//   if (channel >= 0 && channel <= 5)
//     GPIOD->ODR = channel;
// }

// Scale from xLen*yLen to xRange*yRange
// xRange set to 400
// yRange set to 250
void Spectrum_Scale(float32_t *src, uint8_t *dst, uint32_t xBegin, uint32_t xLen, uint32_t yBegin, uint32_t yLen)
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

void Spectrum_Sweep_UpdateFreq()
{
  uint8_t tmp[10];
  uint32_t val;
  val = Spectrum_Sweep_freq * 1000 + 0.5;
  myitoa(val, tmp, 10);
  MyUART_WriteStr(&uart2, "curF.val=");
  MyUART_WriteStr(&uart2, tmp);
  MyUART_WriteStr(&uart2, "\xFF\xFF\xFF");
}

uint8_t Spectrum_WaitResponse(uint8_t ch, uint16_t timeout)
{
  uint8_t rxBuf[15];
  rxBuf[0] = 0x00;
  while (timeout--)
  {
    if (rxBuf[0] == ch)
      return 1;
    Delay_ms(1);
    MyUART_Read(&uart2, rxBuf, 4);
  }
  return 0;
}

void Spectrum_SendCMD(char *str)
{
  MyUART_WriteStr(&uart2, str);
  MyUART_WriteStr(&uart2, "\xFF\xFF\xFF");
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim2)
  {
    if (Spectrum_Sweep_freq >= Spectrum_Sweep_end)
    {
      HAL_TIM_Base_Stop_IT(&htim2);
      return;
    }
    ADF4351_SetFreq(&PLLConfig, Spectrum_Sweep_freq);
    Spectrum_Sweep_freq += Spectrum_Sweep_step;
    if (Spectrum_Sweep_freq > Spectrum_Sweep_end)
    {
      Spectrum_Sweep_freq = Spectrum_Sweep_end;
    }
  }
}
