#ifndef _SWEEP_H_
#define _SWEEP_H_

#include "main.h"

#define SWEEP_LEN_MAX 500
#define ADC_LEN 4096
#define ADC_PIPELINE_DELAY (7 + 1)

#define LBAND_THRE 300
#define LBAND_STEP 10
#define MBAND_THRE 20000
#define MBAND_STEP 100
#define HBAND_THRE 100000
#define HBAND_STEP 2000
#define UBAND_THRE 3000000
#define UBAND_STEP 50000

int32_t phaseDetect(uint32_t len, uint8_t filter);
uint32_t measure(double freq, uint16_t* amp1, uint16_t* amp2, int32_t* phase);

#endif