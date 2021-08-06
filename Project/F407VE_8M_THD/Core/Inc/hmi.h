#ifndef _HMI_H_
#define _HMI_H_

#include "main.h"

void HMI_Process();
void HMI_PageInit();
void HMI_Scale(float32_t *src, uint8_t *dst, uint32_t xBegin, uint32_t xLen, uint32_t yBegin, uint32_t yLen);
uint8_t HMI_WaitResponse(uint8_t ch, uint16_t timeout);
void HMI_DoFFT(uint8_t isAC);
void HMI_SendCMD(char *str);

void HMI_THDPage();
void HMI_WavePage();
void HMI_SpectrumPage();

void HMI_THDInst();
void HMI_WaveInst();
void HMI_SpectrumInst();

void HMI_THDInit();
void HMI_WaveInit();
void HMI_SpectrumInit();

void HMI_THD_SetChannel(uint8_t channel);
void HMI_THD_UpdateHarmony();
void HMI_THD_UpdateSelection();
void HMI_THD_UpdateLabel();
double HMI_THD_GetMovingAverage(double input);

void HMI_Spectrum_UpdateWindow();
void HMI_Spectrum_SetWindow();

#endif