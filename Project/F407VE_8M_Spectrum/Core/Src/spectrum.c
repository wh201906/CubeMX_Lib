#include "spectrum.h"

#include "DELAY/delay.h"
#include "ADF4351/adf4351.h"
#include "AD7190/ad7190.h"
#include "UTIL/util.h"
#include "arm_math.h"
#include "tim.h"

#define SPECTRUM_PAGE_MAIN '0'
#define SPECTRUM_PAGE_SPECTRUM '1'
#define SPECTRUM_PAGE_SWEEP '2'

uint8_t displayBuf[401];
float32_t val[401];

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
    Spectrum_SpectrumPage();
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
    Spectrum_SpectrumInst();
  else if (Spectrum_Buf[0] == SPECTRUM_PAGE_SWEEP)
    Spectrum_SweepInst();
}

void Spectrum_PageInit()
{
  if (Spectrum_CurrentPage == SPECTRUM_PAGE_SPECTRUM)
    Spectrum_SpectrumInit();
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

void Spectrum_SpectrumInit()
{
  ADF4351_Init();
  ADF4351_SetCLKConfig(&PLLConfig, 100, 25, 0, 1, 32, 0.001);
  ADF4351_WriteCLKConfig(&PLLConfig);

  AD7190_Init(3.0);
  AD7190_SetPolar(AD7190_POLAR_UNIPOLAR);
  AD7190_SetChannel(AD7190_CH_1G);
  AD7190_SetFS(1);
  AD7190_SetCLKSource(AD7190_CLK_EXT_12);
  AD7190_SetGain(AD7190_GAIN_1);
}

void Spectrum_SweepPage()
{
  Spectrum_Sweep_UpdateFreq();
  Delay_ms(100);
}

void Spectrum_SpectrumPage()
{
  uint8_t buf[15], aveNum;
  float32_t maxVal, minVal;
  uint32_t i, j;
  double begin, end, curr;
  begin = 90.7;
  end = 110.7;
  aveNum = 5;
  i = 0;
  for (curr = begin; curr < end; curr += 0.05)
  {
    val[i] = 0;
    ADF4351_SetFreq(&PLLConfig, curr);
    Delay_ms(2);
    for (j = 0; j < aveNum; j++)
    {
      val[i] += AD7190_GetVoltage() * 85;
      Delay_us(3);
    }
    val[i] /= aveNum;
    i++;
  }
  arm_min_f32(val, 400, &minVal, &i);
  arm_max_f32(val, 400, &maxVal, &j);
  arm_offset_f32(val, -minVal, val, 400);
  //arm_scale_f32(val, 248.0 / (maxVal - minVal), val, 400);
  for (i = 0; i < 400; i++)
  {
    if(i == j && maxVal - minVal > 40)
      displayBuf[i] = 248;
    else
      displayBuf[i] = val[i] * 0.2;
  }
  Spectrum_SendCMD("ref_stop");
  Spectrum_SendCMD("cle 1,0");
  Spectrum_SendCMD("addt 1,0,400");
  Spectrum_WaitResponse(0xFE, 30);

  for (i = 0; i < 400; i++)
    MyUART_WriteChar(&uart2, displayBuf[i]);
  Spectrum_WaitResponse(0xFD, 30);

  Spectrum_SendCMD("ref_star");
  MyUART_WriteStr(&uart2, "state.txt=\"Freq: ");
  myftoa(j *0.05+80.0 - 0.05, buf);
  MyUART_WriteStr(&uart2, buf);
  MyUART_WriteStr(&uart2, "\"\xFF\xFF\xFF");
  Delay_ms(200);
}

void Spectrum_SpectrumInst()
{
}

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
    Spectrum_Sweep_freq = Spectrum_Sweep_begin - Spectrum_Sweep_step;
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
    Spectrum_Sweep_freq += Spectrum_Sweep_step;
    ADF4351_SetFreq(&PLLConfig, Spectrum_Sweep_freq);
    if (Spectrum_Sweep_freq >= Spectrum_Sweep_end || Spectrum_Sweep_end - Spectrum_Sweep_freq < 0.00001)
    {
      HAL_TIM_Base_Stop_IT(&htim2);
      return;
    }
  }
}
