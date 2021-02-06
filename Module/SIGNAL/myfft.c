#include "myfft.h"
#include "math.h"

float32_t fftOutput[MYFFT_LENGTH];
arm_rfft_fast_instance_f32 fftInst;
double sampRate;
#if MYFFT_USE_HANNING
float32_t hanningWindow[MYFFT_LENGTH];
float32_t fftPreProcess[MYFFT_LENGTH];
#endif

void MyFFT_Init(double sampleRate)
{
  uint16_t i;
  arm_rfft_fast_init_f32(&fftInst, MYFFT_LENGTH);

  // this value doesn't affect FFT transform
  // set sampleRate to any value if you don't use MyFFT_GetPeakFreq()
  MyFFT_SetSampleRate(sampleRate);

#if MYFFT_USE_HANNING
  // see https://www.mathworks.com/help/signal/ref/hann.html
  // hann(MYFFT_LENGTH, 'peroidic')
  hanningWindow[0] = 0;
  for (i = 1; i < MYFFT_LENGTH / 2 + 1; i++)
  {
    hanningWindow[i] = 0.5 * (1 - arm_cos_f32(2 * PI * i / MYFFT_LENGTH));
    hanningWindow[MYFFT_LENGTH - i] = hanningWindow[i];
  }

#endif
}

void MyFFT_SetSampleRate(double sampleRate)
{
  sampRate = sampleRate;
}

void MyFFT_CalcInPlace(float32_t *data)
{
  MyFFT_Calc(data, data);
}

void MyFFT_Calc(float32_t *input, float32_t *output)
{
#if MYFFT_USE_HANNING
  arm_mult_f32(input, hanningWindow, fftPreProcess, MYFFT_LENGTH);
  arm_rfft_fast_f32(&fftInst, fftPreProcess, fftOutput, 0);
#else
  arm_rfft_fast_f32(&fftInst, input, fftOutput, 0);
#endif
  arm_cmplx_mag_f32(fftOutput, output, MYFFT_LENGTH / 2);
  // output[0] /= 2; // for Amplitude
}

double MyFFT_GetPeakFreq(float32_t *data, uint16_t len)
{
  uint32_t i;
  float32_t val;
  arm_max_f32(data, len, &val, &i);
  return (double)i * sampRate / MYFFT_LENGTH;
}

void MyFFT_GenerateArray(float32_t *data, double sampleRate, double *freqArray, double *ampArray, uint32_t arrayLen)
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
      *(data + i) += *(ampArray + j) * arm_cos_f32(2 * PI * (*(freqArray + j)) * i / sampleRate);
    // use cos() to handle 0Hz(cos(0)=1, for DC)
  }
}