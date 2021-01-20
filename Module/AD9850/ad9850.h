#ifndef _AD9850_H
#define _AD9850_H

#include "main.h"
#include "spi.h"
#include "DELAY/delay.h"

#define AD9850_FQUD_PIN GPIO_PIN_15
#define AD9850_RESET_PIN GPIO_PIN_13

#define AD9850_FQUD_GPIO GPIOE
#define AD9850_RESET_GPIO GPIOE

#define POW2_32 4294967296u
#define AD9850_CLK 125000000u

uint32_t AD9850_Freq2Reg(double freq);
double AD9850_GetActuralFreq(uint32_t regVal);
uint32_t AD9850_GetCurrentFreqReg(void);
void AD9850_SetFreq(double freq);

uint8_t AD9850_Phase2Reg(double phase);
double AD9850_GetActuralPhase(uint8_t regVal);
uint8_t AD9850_GetCurrentPhaseReg(void);
void AD9850_SetPhase(double phase);

void AD9850_SetPowerDown(uint8_t isPowerDown);
uint8_t AD9850_IsPowerDown(void);

void AD9850_SendRaw(uint8_t* data);
void AD9850_Reset(void);

#endif