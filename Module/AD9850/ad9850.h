#ifndef _AD9850_H
#define _AD9850_H

#include "main.h"
#include "DELAY/delay.h"

#define AD9850_RESET_PIN GPIO_PIN_10
#define AD9850_FQUD_PIN GPIO_PIN_11
#define AD9850_WCLK_PIN GPIO_PIN_12
#define AD9850_D7_PIN GPIO_PIN_15
#define AD9850_D6_PIN GPIO_PIN_14
#define AD9850_D5_PIN GPIO_PIN_13
#define AD9850_D4_PIN GPIO_PIN_12
#define AD9850_D3_PIN GPIO_PIN_11
#define AD9850_D2_PIN GPIO_PIN_10
#define AD9850_D1_PIN GPIO_PIN_9
#define AD9850_D0_PIN GPIO_PIN_8

#define AD9850_RESET_GPIO GPIOB
#define AD9850_FQUD_GPIO GPIOB
#define AD9850_WCLK_GPIO GPIOB
#define AD9850_D_GPIO GPIOE

#define POW2_32 4294967296u
#define AD9850_CLK 125000000u
#define AD9850_PHASE_DELTA 0.0889

uint32_t AD9850_Freq2Reg(double freq);
double AD9850_GetActuralFreq(uint32_t regVal);
uint32_t AD9850_GetCurrentFreqReg(void);
void AD9850_SetFreq(double freq);

uint8_t AD9850_Phase2Reg(double phase);
double AD9850_GetActuralPhase(uint8_t regVal);
uint8_t AD9850_GetCurrentPhaseReg(void);
void AD9850_SetPhase(double phase);

void AD9850_SetPowerDown(uint8_t isPowerDown);

void AD9850_Update(void);
void AD9850_SendByte(uint8_t data);
void AD9850_ModeUpdate(void);
void AD9850_Reset(void);
void AD9850_Init(void);

#endif