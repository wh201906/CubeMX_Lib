#ifndef _SIGPARA_H
#define _SIGPARA_H

#include "main.h"
#include "tim.h"
#include "arm_math.h"

float32_t SigPara_RMS(const float32_t *data, uint32_t len);
void SigPara_Freq_LF_Init(void);
double SigPara_Freq_LF(void);

#endif