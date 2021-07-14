#ifndef _ADF4351_H_
#define _ADF4351_H_
#include "main.h"

#define ADF4351_RF_OFF	((uint32_t)0XEC801C)

void ADF4351_Init(void);
void ADF4351_Write(uint32_t val);
void ADF4351_Reg_Init(void);
void ADF4351_SetFreq(float freq);		//	(xx.x) M Hz

#endif

