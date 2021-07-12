#include "myfft.h"

float32_t fftOutput[MYFFT_LENGTH];
arm_rfft_fast_instance_f32 fftInst;
double MyFFT_sampRate;

#if MYFFT_USE_WINDOW
float32_t windowCoef[MYFFT_LENGTH];
float32_t fftPreProcess[MYFFT_LENGTH];
uint8_t MyFFT_isUsingWindow = 0; // speed up if no window is used

float MyFFT_Flat_a0 = 0.21557895f;
float MyFFT_Flat_a1 = 0.41663158f;
float MyFFT_Flat_a2 = 0.277263158f;
float MyFFT_Flat_a3 = 0.083578947f;
float MyFFT_Flat_a4 = 0.006947368f;
float MyFFT_Black_a0 = 0.42f;
float MyFFT_Black_a1 = 0.5f;
float MyFFT_Black_a2 = 0.08f;

void MyFFT_NoWindow(void)
{
  // arm_fill_f32(1, windowCoef, MYFFT_LENGTH);
  MyFFT_isUsingWindow = 0;
}

void MyFFT_HannWindow(void)
{
  // see https://www.mathworks.com/help/signal/ref/hann.html
  // hann(MYFFT_LENGTH, 'periodic')
  uint16_t i;
  windowCoef[0] = 0;
  for (i = 1; i < MYFFT_LENGTH / 2 + 1; i++)
  {
    windowCoef[i] = 0.5 * (1 - arm_cos_f32(2 * PI * i / MYFFT_LENGTH));
    windowCoef[i] = 0.54 - 0.46 * arm_cos_f32(2 * PI * i / MYFFT_LENGTH);
    windowCoef[MYFFT_LENGTH - i] = windowCoef[i];
  }
  MyFFT_isUsingWindow = 1;
}

void MyFFT_HammingWindow(void)
{
  // hamming(MYFFT_LENGTH, 'periodic')
  uint16_t i;
  windowCoef[0] = 0.08;
  for (i = 1; i < MYFFT_LENGTH / 2 + 1; i++)
  {
    windowCoef[i] = 0.5 * (1 - arm_cos_f32(2 * PI * i / MYFFT_LENGTH));
    windowCoef[MYFFT_LENGTH - i] = windowCoef[i];
  }
  MyFFT_isUsingWindow = 1;
}

void MyFFT_FlattopWindow(void)
{
  // flattopwin(MYFFT_LENGTH, 'periodic')
  uint16_t i;
  windowCoef[0] = MyFFT_Flat_a0 - MyFFT_Flat_a1 + MyFFT_Flat_a2 - MyFFT_Flat_a3 + MyFFT_Flat_a4;
  for (i = 1; i < MYFFT_LENGTH / 2 + 1; i++)
  {
    windowCoef[i] = MyFFT_Flat_a0;
    windowCoef[i] -= MyFFT_Flat_a1 * arm_cos_f32(2 * PI * i / MYFFT_LENGTH);
    windowCoef[i] += MyFFT_Flat_a2 * arm_cos_f32(4 * PI * i / MYFFT_LENGTH);
    windowCoef[i] -= MyFFT_Flat_a3 * arm_cos_f32(6 * PI * i / MYFFT_LENGTH);
    windowCoef[i] += MyFFT_Flat_a4 * arm_cos_f32(8 * PI * i / MYFFT_LENGTH);
    windowCoef[MYFFT_LENGTH - i] = windowCoef[i];
  }
  MyFFT_isUsingWindow = 1;
}

void MyFFT_BlackmanWindow(void)
{
  // blackman(L, 'periodic')
  uint16_t i;
  windowCoef[0] = MyFFT_Black_a0 - MyFFT_Black_a1 + MyFFT_Black_a2;
  for (i = 1; i < MYFFT_LENGTH / 2 + 1; i++)
  {
    windowCoef[i] = MyFFT_Black_a0;
    windowCoef[i] -= MyFFT_Black_a1 * arm_cos_f32(2 * PI * i / MYFFT_LENGTH);
    windowCoef[i] += MyFFT_Black_a2 * arm_cos_f32(4 * PI * i / MYFFT_LENGTH);
    windowCoef[MYFFT_LENGTH - i] = windowCoef[i];
  }
  MyFFT_isUsingWindow = 1;
}

void MyFFT_TriangWindow(void)
{
  // the L is always even
  // triang(L)
  uint16_t i;
  for (i = 0; i < MYFFT_LENGTH / 2; i++)
  {
    windowCoef[i] = (2.0 * i + 1) / MYFFT_LENGTH;
    windowCoef[MYFFT_LENGTH - 1 - i] = windowCoef[i];
  }
  MyFFT_isUsingWindow = 1;
}

#endif

void MyFFT_Init(double sampleRate)
{
  arm_rfft_fast_init_f32(&fftInst, MYFFT_LENGTH);

  // this value doesn't affect FFT transform
  // set sampleRate to any value if you don't use MyFFT_GetPeakFreq()
  MyFFT_SetSampleRate(sampleRate);
#if MYFFT_USE_WINDOW
  MyFFT_NoWindow();
#endif
}

void MyFFT_SetSampleRate(double sampleRate)
{
  MyFFT_sampRate = sampleRate;
}

void MyFFT_CalcInPlace(float32_t *data)
{
  MyFFT_Calc(data, data);
}

void MyFFT_Calc(float32_t *input, float32_t *output)
{
#if MYFFT_USE_WINDOW
  if (MyFFT_isUsingWindow)
  {
    arm_mult_f32(input, windowCoef, fftPreProcess, MYFFT_LENGTH);
    arm_rfft_fast_f32(&fftInst, fftPreProcess, fftOutput, 0);
  }
  else
    arm_rfft_fast_f32(&fftInst, input, fftOutput, 0);
#else
  arm_rfft_fast_f32(&fftInst, input, fftOutput, 0);
#endif
  arm_cmplx_mag_f32(fftOutput, output, MYFFT_LENGTH / 2);
  output[0] /= 2;                                                            // amplitude correction
  arm_scale_f32(output, (double)2 / MYFFT_LENGTH, output, MYFFT_LENGTH / 2); // amplitude correction
}

double MyFFT_GetPeakFreq(float32_t *data, uint16_t len)
{
  uint32_t i;
  float32_t val;
  arm_max_f32(data, len, &val, &i);
  return (double)i * MyFFT_sampRate / MYFFT_LENGTH;
}

double MyFFT_THD(float32_t *data, uint16_t len, uint16_t offset, uint8_t nThre)
{
  double thd;
  uint8_t i, tmp;
  uint32_t baseI;
  float32_t baseV, harmonyV = 0, noiseV, threshold = 0.001;
  float32_t currHarmony;
  uint16_t range = 1;
  arm_min_f32(data + offset, len - offset, &noiseV, &baseI); //no arm_min_no_idx_f32() now, baseI will be overrided
  arm_max_f32(data + offset, len - offset, &baseV, &baseI);
  baseI += offset;
  threshold *= (baseV - noiseV);
  threshold += noiseV;
  for (i = 2; i <= nThre; i++)
  {
    if (i & 1u)
      range++;
    tmp = baseI * i - range;
    if (tmp < offset)
      tmp = offset;
    arm_max_no_idx_f32(data + tmp, 2 * range, &currHarmony);
    if (currHarmony >= threshold)
      harmonyV += currHarmony * currHarmony;
  }
  arm_sqrt_f32(harmonyV, &harmonyV);
  thd = (double)harmonyV / baseV;
  return (thd > 1 ? 1.0 : thd);
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