#include "sweep.h"
#include "arm_math.h"
#include "AD9834/ad9834.h"
#include "PARAIO/paraio.h"

extern uint16_t adcBuf[ADC_LEN + ADC_PIPELINE_DELAY];
extern uint16_t adcBuf2[ADC_LEN + ADC_PIPELINE_DELAY];
extern double freq[SWEEP_LEN_MAX], amp[SWEEP_LEN_MAX], ph[SWEEP_LEN_MAX];

double LBand_ref = 1.0;
double MBand_ref = 1.0;
double HBand_ref = 1.0;
double UBand_ref = 1.0;

// for LF, set filter to 200
// for HF, reduce filter value
int32_t phaseDetect(uint32_t len, uint8_t filter)
{
  uint16_t mean1, mean2;
  uint32_t i, z1 = 0, z2 = 0;
  uint16_t *bufPtr1 = adcBuf + ADC_PIPELINE_DELAY;
  uint16_t *bufPtr2 = adcBuf2 + ADC_PIPELINE_DELAY;

  // find virtual zero, ignore the different amplitude
  arm_mean_q15(bufPtr1, len, &mean1);
  arm_mean_q15(bufPtr2 + ADC_PIPELINE_DELAY, len, &mean2);

  // find zero-crossing
  //printf("mean1:%d\r\n", mean1);
  for (i = 0; i < len - 1; i++)
  {
    if ((bufPtr1[i] - mean1 & 0x8000) && !(bufPtr1[i + 1] - mean1 & 0x8000)) // posedge
    {
      // filter
      // the adcBuf contains at least 1 complete period
      if (i > filter && bufPtr1[i - filter] > bufPtr1[i])
        continue;
      else if (i < len - filter && bufPtr1[i + filter] < bufPtr1[i])
        continue;

      z1 = i;
      //printf("i:%d,1[i]:%d,1[i+1]:%d\r\n", i, adcBuf[i] & 0xFFF, adcBuf[i+1] & 0xFFF);
      break;
    }
  }
  //printf("mean2:%d\r\n", mean2);
  for (i = 0; i < len - 1; i++)
  {
    if ((bufPtr2[i] - mean2 & 0x8000) && !(bufPtr2[i + 1] - mean2 & 0x8000)) // posedge
    {
      if (i > filter && bufPtr2[i - filter] >= bufPtr2[i])
        continue;
      else if (i < len - filter && bufPtr2[i + filter] <= bufPtr2[i])
        continue;

      z2 = i;
      //printf("i:%d,2[i]:%d,2[i+1]:%d\r\n", i, adcBuf2[i] & 0xFFF, adcBuf2[i+1] & 0xFFF);
      break;
    }
  }
  return ((int32_t)z1 - (int32_t)z2 + len) % len;
}

uint32_t measure(double freq, uint16_t *amp1, uint16_t *amp2, int32_t *phase)
{
  uint8_t phaseFilter = 0;
  uint32_t len, tmp, i;
  uint16_t maxVal, minVal;

  if (freq < 150000 && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) != GPIO_PIN_SET)
  {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
    Delay_ms(50);
  }
  else if (freq >= 150000 && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) != GPIO_PIN_RESET)
  {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
    Delay_ms(50);
  }
  // HF: sample for 50 periods
  tmp = 2; // 168M / ((arr + 1) * (psc + 1)) = 14M < 0.25APB2 clock speed(21M)
  htim8.Instance->PSC = tmp;
  tmp = 168000000 / (__HAL_TIM_GET_AUTORELOAD(&htim8) + 1) / (tmp + 1); // current sample rate
  tmp /= (uint32_t)freq;                                                // dot per period

  if (tmp > ADC_LEN / 5)
    phaseFilter = 200;

  len = tmp * 50; // dot for 50 periods
  if (len > ADC_LEN)
    len = ADC_LEN;

  // LF: sample for 1 period
  // for sine wave:
  // slope_max = max(sin'(x)) = Vpp
  // x_step: 1 / ADC_LEN
  // offset_max < slope_max * x_step
  // offset_max < Vpp / ADC_LEN
  if (tmp > ADC_LEN)
  {
    // sample 1.01% period
    tmp = freq * ADC_LEN / 1.01;                      // required sample rate
    tmp = 168000000 / tmp + 1;                     // (arr + 1) * (psc + 1)
    tmp /= (__HAL_TIM_GET_AUTORELOAD(&htim8) + 1); // psc + 1
    htim8.Instance->PSC = tmp;
  }
  
  AD9834_SetFreq(freq, 0);
  Delay_us(1);
  htim8.Instance->EGR = TIM_EGR_UG; // update arr and psc
  ParaIO_Start_In_Sync(adcBuf, len + ADC_PIPELINE_DELAY, adcBuf2, len + ADC_PIPELINE_DELAY);
  while (!ParaIO_IsTranferCompleted_In2())
    ;

  // the width of adc result is 12bit. So the MSB of uint16_t is always zero.
  // q15 works there.

  // for the second AD9226:
  // D11~D0 -> PE13~PE2
  arm_shift_q15(adcBuf2 + ADC_PIPELINE_DELAY, -2, adcBuf2 + ADC_PIPELINE_DELAY, len);
  for (i = 0; i < len; i++)
  {
    adcBuf[i + ADC_PIPELINE_DELAY] &= 0xFFF;
    adcBuf2[i + ADC_PIPELINE_DELAY] &= 0xFFF;
  }

  // AD9280 has a Pipeline Delay of 7 cycles
  arm_max_q15(adcBuf + ADC_PIPELINE_DELAY, len, &maxVal, &tmp);
  arm_min_q15(adcBuf + ADC_PIPELINE_DELAY, len, &minVal, &tmp);
  *amp1 = maxVal - minVal;
  arm_max_q15(adcBuf2 + ADC_PIPELINE_DELAY, len, &maxVal, &tmp);
  arm_min_q15(adcBuf2 + ADC_PIPELINE_DELAY, len, &minVal, &tmp);
  *amp2 = maxVal - minVal;
  if (phase != NULL)
    *phase = phaseDetect(len, phaseFilter);
  return len;
}

uint32_t sweep(double start, double stop, AmpMean *ampMean)
{
  double currFreq = start;
  uint16_t amp1, amp2;
  int32_t phase;
  uint32_t i = 0, j = 0, tmp = 0;
  
  // necessary dummy read when the delta_freq is too high
  measure(currFreq, &amp1, &amp2, &phase);
  for (; currFreq < LBAND_THRE && currFreq <= stop; currFreq += LBAND_STEP)
  {
    measure(currFreq, &amp1, &amp2, &phase);
    
    // check waveform
//    if(currFreq == start)
//    {
//      for(j = 0; j < ADC_LEN; j++)
//        printf("%d,%d\r\n",adcBuf[j], adcBuf2[j]);
//      j = 0;
//    }
    freq[i] = currFreq;
    amp[i] = (double)amp2 / amp1 / LBand_ref;
    ph[i] = phase;
    i++;
  }
  if (ampMean != NULL)
  {
    ampMean->L = 0;
    for (j = tmp; j < i; j++)
      ampMean->L += amp[j];
    ampMean->L /= i - tmp;
  }
  tmp = i;
  for (; currFreq < MBAND_THRE && currFreq <= stop; currFreq += MBAND_STEP)
  {
    measure(currFreq, &amp1, &amp2, &phase);
    freq[i] = currFreq;
    amp[i] = (double)amp2 / amp1 / MBand_ref;
    ph[i] = phase;
    i++;
  }
  if (ampMean != NULL)
  {
    ampMean->M = 0;
    for (j = tmp; j < i; j++)
      ampMean->M += amp[j];
    ampMean->M /= i - tmp;
  }
  tmp = i;
  for (; currFreq < HBAND_THRE && currFreq <= stop; currFreq += HBAND_STEP)
  {
    measure(currFreq, &amp1, &amp2, &phase);
    freq[i] = currFreq;
    amp[i] = (double)amp2 / amp1 / HBand_ref;
    ph[i] = phase;
    i++;
  }
  if (ampMean != NULL)
  {
    ampMean->H = 0;
    for (j = tmp; j < i; j++)
      ampMean->H += amp[j];
    ampMean->H /= i - tmp;
  }
  tmp = i;
  for (; currFreq <= UBAND_THRE && currFreq <= stop; currFreq += UBAND_STEP)
  {
    measure(currFreq, &amp1, &amp2, &phase);
    freq[i] = currFreq;
    amp[i] = (double)amp2 / amp1 / UBand_ref;
    ph[i] = phase;
    i++;
  }
  if (ampMean != NULL)
  {
    ampMean->U = 0;
    for (j = tmp; j < i; j++)
      ampMean->U += amp[j];
    ampMean->U /= i - tmp;
  }
  return i;
}

void calibrate(double start, double stop)
{
  AmpMean ref;

  LBand_ref = 1;
  MBand_ref = 1;
  HBand_ref = 1;
  UBand_ref = 1;

  sweep(start, stop, &ref);

  LBand_ref = ref.L;
  MBand_ref = ref.M;
  HBand_ref = ref.H;
  UBand_ref = ref.U;
}

double calc_R(double amplitude) // ohm
{
  return amplitude / (1 - amplitude) * R_REF;
}

double calc_C(double frequency) // uF
{
  return 500000.0 / (PI * frequency * R_REF);
}

double calc_L(double frequency) // uH
{
  return (500000.0 * R_REF) / (PI * frequency);
}

double freqSearch_helper(double currFreq)
{
  uint16_t amp1, amp2;
  double result;
  measure(currFreq, &amp1, &amp2, NULL);
  result = (double)amp2 / amp1;
  if (currFreq < LBAND_THRE)
    result /= LBand_ref;
  else if (currFreq < MBAND_THRE)
    result /= MBand_ref;
  else if (currFreq < HBAND_THRE)
    result /= HBand_ref;
  else
    result /= UBand_ref;
  return result;
}

// monotonic
double freqSearch(double freqL, double freqH, double ampL, double ampH, double requiredAmp, double precision)
{
  double freqM, ampM;
  uint8_t isIncreasing;
  precision *= freqH - freqL; // end condition
  isIncreasing = (ampL < ampH);
  while (freqH - freqL > precision && fabs(ampL - requiredAmp) > 0.01 && fabs(ampH - requiredAmp) > 0.01)
  {
    printf("L:%f,H:%f,aL:%f,aH:%f\r\n", freqL, freqH, ampL, ampH);
    freqM = (freqL + freqH) / 2;
    ampM = freqSearch_helper(freqM);
    if (requiredAmp <= ampM && isIncreasing || requiredAmp > ampM && !isIncreasing)
    {
      freqH = freqM;
      ampH = ampM;
    }
    else
    {
      freqL = freqM;
      ampL = ampM;
    }
  }
  return ((fabs(ampL - requiredAmp) < fabs(ampH - requiredAmp)) ? freqL : freqH);
}