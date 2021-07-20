#ifndef _SPECTRUM_H_
#define _SPECTRUM_H_

#include "main.h"

void Spectrum_Process();
void Spectrum_PageInit();

void Spectrum_SweepInit();
void Spectrum_SweepPage();
void Spectrum_SweepInst();
void Spectrum_Sweep_UpdateFreq();

void Spectrum_SendCMD(char *str);
#endif