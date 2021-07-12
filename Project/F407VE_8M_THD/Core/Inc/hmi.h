#ifndef _HMI_H_
#define _HMI_H_

#include "main.h"

void HMI_Process();
void HMI_PageInit();

void HMI_THDPage();
void HMI_WavePage();
void HMI_SpectrumPage();

void HMI_THDInst();
void HMI_WaveInst();
void HMI_SpectrumInst();

void HMI_THD_SetChannel(uint8_t channel);
void HMI_THD_UpdateLabel();

#endif