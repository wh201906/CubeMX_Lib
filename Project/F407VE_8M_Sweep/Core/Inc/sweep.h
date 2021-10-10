#ifndef _SWEEP_H_
#define _SWEEP_H_

#include "main.h"

#define SWEEP_LEN_MAX 100
#define ADC_LEN 4096
#define ADC_PIPELINE_DELAY (7 + 1)
#define R_REF 226.7
#ifndef PI
#define PI 3.14159265
#endif

#define LBAND_THRE 300
#define LBAND_STEP 60
#define MBAND_THRE 20000
#define MBAND_STEP 2000
#define HBAND_THRE 100000
#define HBAND_STEP 20000
#define UBAND_THRE 3000000
#define UBAND_STEP 200000

typedef struct _AmpMean
{
  double L, M, H, U;
} AmpMean;

int32_t phaseDetect(uint32_t len, uint8_t filter);
uint32_t measure(double freq, uint16_t* amp1, uint16_t* amp2, int32_t* phase);
uint32_t sweep(double start, double stop, AmpMean* ampMean);
void calibrate(double start, double stop);
double calc_R(double amplitude);
double calc_C(double frequency);
double calc_L(double frequency);
double freqSearch(double freqL, double freqR, double ampL, double ampR, double requiredAmp, double precision);

void LED_fast(void);
void LED_slow(void);
void LED_off(void);

#endif