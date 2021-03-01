#include "myfft_q15.h"

q15_t fftOutput[MYFFT_LENGTH];
arm_rfft_instance_q15 fftInst;
double sampRate;
#if MYFFT_USE_HANNING
q15_t hanningWindow[MYFFT_LENGTH];
q15_t fftPreProcess[MYFFT_LENGTH];
#endif

void MyFFT_Init(double sampleRate)
{
  uint16_t i;
  arm_rfft_init_q15(&fftInst, MYFFT_LENGTH, 0, 1);

  // this value doesn't affect FFT transform
  // set sampleRate to any value if you don't use MyFFT_GetPeakFreq()
  MyFFT_SetSampleRate(sampleRate);

#if MYFFT_USE_HANNING
  // see https://www.mathworks.com/help/signal/ref/hann.html
  // hann(MYFFT_LENGTH, 'peroidic')
  float32_t temp[MYFFT_LENGTH / 2 + 1];
  temp[0] = 0;
  for (i = 1; i < MYFFT_LENGTH / 2 + 1; i++)
    temp[i] = 0.5 * (1 - arm_cos_q15(2 * PI * i / MYFFT_LENGTH));
  arm_float_to_q15(temp, hanningWindow, MYFFT_LENGTH / 2 + 1);
  for (i = 1; i < MYFFT_LENGTH / 2 + 1; i++)
    hanningWindow[MYFFT_LENGTH - i] = hanningWindow[i];

#endif
}

void MyFFT_SetSampleRate(double sampleRate)
{
  sampRate = sampleRate;
}

void MyFFT_CalcInPlace(q15_t *data)
{
  MyFFT_Calc(data, data);
}

void MyFFT_Calc(q15_t *input, q15_t *output)
{
#if MYFFT_USE_HANNING
  arm_mult_q15(input, hanningWindow, fftPreProcess, MYFFT_LENGTH);
  arm_rfft_q15(&fftInst, fftPreProcess, fftOutput);
#else
  arm_rfft_q15(&fftInst, input, fftOutput);
#endif
  // need some modify
  arm_cmplx_mag_q15(fftOutput, output, MYFFT_LENGTH / 2);
  output[0] /= 2;                                                            // amplitude correction
  arm_scale_q15(output, (double)2 / MYFFT_LENGTH, output, MYFFT_LENGTH / 2); // amplitude correction
}

// need some modify
double MyFFT_GetPeakFreq(q15_t *data, uint16_t len)
{
  uint32_t i;
  q15_t val;
  arm_max_q15(data, len, &val, &i);
  return (double)i * sampRate / MYFFT_LENGTH;
}

// need some modify
void MyFFT_GenerateArray(q15_t *data, double sampleRate, double *freqArray, double *ampArray, uint32_t arrayLen)
{
  // Tsample = MYFFT_LENGTH / sampleRate
  // t/div = Tsample / MYFFT_LENGTH = 1/sampleRate
  // f/div = (sampleRate/2) / (MYFFT_LENGTH/2) = sampleRate / MYFFT_LENGTH
  // x = Asin(wt)
  // x = Asin((2*pi) * f * t)
  // x = Asin((2*pi) * f * (i * (t/div)))
  uint16_t i;
  uint32_t j;
  for (i = 0; i < MYFFT_LENGTH; i++)
  {
    *(data + i) = 0;
    // PI has been defined in arm_math.h
    for (j = 0; j < arrayLen; j++)
      *(data + i) += *(ampArray + j) * arm_cos_q15(2 * PI * (*(freqArray + j)) * i / sampleRate);
    // use cos() to handle 0Hz(cos(0)=1, for DC)
  }
}