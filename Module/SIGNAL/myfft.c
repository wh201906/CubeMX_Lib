#include "myfft.h"

float32_t fftOutput[MYFFT_LENGTH];
arm_rfft_fast_instance_f32 fftInst;
double sampRate;

void MyFFT_Init(double sampleRate)
{
  arm_rfft_fast_init_f32(&fftInst, MYFFT_LENGTH);
  // this value doesn't affect FFT transform
  // set sampleRate to any value if you don't use MyFFT_GetPeakFreq()
  MyFFT_SetSampleRate(sampleRate);
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
  arm_rfft_fast_f32(&fftInst, input, fftOutput, 0);
  arm_cmplx_mag_f32(fftOutput, output, MYFFT_LENGTH / 2);
  // output[0] /= 2; // for Amplitude
}

double MyFFT_GetPeakFreq(float32_t *data)
{
  uint16_t i;
  float32_t val;
  arm_max_f32(data + 1, MYFFT_LENGTH / 2 - 1, &val, &i); // ignore DC
  return i;                                            // need modify there
}