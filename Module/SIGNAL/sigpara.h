#ifndef _SIGPARA_H
#define _SIGPARA_H

#include "main.h"
#include "tim.h"
#include "arm_math.h"

float32_t SigPara_RMS(const float32_t *data, uint32_t len);
static void SigPara_Freq_LF_TIM_Init(void);
static void SigPara_Freq_LF_GPIO_Init(void);
static void SigPara_Freq_LF_DMA_Init(void);
void SigPara_Freq_LF_Init(void);
double SigPara_Freq_LF(void);
void SigPara_Freq_HF_Init(void);
double SigPara_Freq_HF(void);

#endif