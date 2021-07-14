#ifndef _SIGPARA_H
#define _SIGPARA_H

#include "main.h"
#include "DELAY/delay.h"
#include "tim.h"
#include "arm_math.h"

#define SIGPARA_HTIM1_CLK 84000000.0
#define SIGPARA_HTIM2_CLK 84000000.0

float32_t SigPara_RMS(const float32_t *data, uint32_t len);

static void SigPara_Freq_LF_TIM_Init(void);
static void SigPara_Freq_LF_GPIO_Init(void);
static void SigPara_Freq_LF_DMA_Init(void);
void SigPara_Freq_LF_Init(void);
double SigPara_Freq_LF(uint32_t timeout);

static void SigPara_Freq_HF_TimerTIM_Init(void);
static void SigPara_Freq_HF_CounterTIM_Init(void);
static void SigPara_Freq_HF_GPIO_Init(void);
void SigPara_Freq_HF_Init(void);
double SigPara_Freq_HF(uint32_t countTimes, uint16_t P);

uint64_t SigPara_Freq_Auto_SetMinPrecision(double permillage);
void SigPara_Freq_Auto_SetMaxTimeout(uint32_t ms);
double SigPara_Freq_Auto(void); // Frequency measurement with auto range

static void SigPara_PWM_TIM_Init(void);
static void SigPara_PWM_GPIO_Init(void);
void SigPara_PWM_Init(void);
double SigPara_PWM(uint32_t timeout, double *freqPtr);

#endif