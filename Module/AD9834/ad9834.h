#ifndef _AD9834_H
#define _AD9834_H

#include "main.h"
#include "spi.h"
#include "DELAY/delay.h"

#define AD9834_NSS_PIN GPIO_PIN_14
#define AD9834_NSS_GPIO GPIOB
#define AD9834_NSS_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()

#ifndef POW2_28
#define POW2_28 268435456u
#endif
#define AD9834_CLK 75000000u
#define AD9834_PHASE_DELTA 11.377777777777778

// the target Register prefix
#define AD9834_REGADDR_MASK 0xE000u
#define AD9834_REGADDR_CTRL 0x0000u
#define AD9834_REGADDR_FREQ0 0x4000u
#define AD9834_REGADDR_FREQ1 0x8000u
#define AD9834_REGADDR_PHASE0 0xC000u
#define AD9834_REGADDR_PHASE1 0xE000u

// the suffix of wave type and SLEEP1/SLEEP12, available in AD9834_ctrlReg
#define AD9834_WAVE_MASK 0x00FFu
#define AD9834_WAVE_SQOFF 0x0000u       // SLEEP12 = 0, (OPBITEN = 0), SIGN/PIB = 0, DIV2 = 0, MODE = 0 to disable SIGN BIT OUT  (compatible)
#define AD9834_WAVE_SQUARE 0x0028u      // SLEEP12 = 0, (OPBITEN = 1, SIGN/PIB = 0, DIV2 = 1, MODE = 0)                          (conflict to Tri)
#define AD9834_WAVE_SQUARE2 0x0020u     // SLEEP12 = 0, (OPBITEN = 1, SIGN/PIB = 0, DIV2 = 0, MODE = 0)                          (conflict to Tri)
#define AD9834_WAVE_SQUARE_COMP 0x0038u // SLEEP12 = 0, (OPBITEN = 1, SIGN/PIB = 1, DIV2 = 1, MODE = 0)                          (conflict to Tri)
#define AD9834_WAVE_DACOFF 0x0040u      // (SLEEP12 = 1), OPBITEN = 0, SIGN/PIB = 0, DIV2 = 0, MODE = 0 to disable DAC           (compatible)
#define AD9834_WAVE_SINE 0x0000u        // SLEEP12 = 0, OPBITEN = 0, SIGN/PIB = 0, DIV2 = 0, (MODE = 0)                          (compatible)
#define AD9834_WAVE_TRI 0x0002u         // SLEEP12 = 0, (OPBITEN = 0), SIGN/PIB = 0, DIV2 = 0, (MODE = 1)                        (conflict to Square)

// freqReg configuring mode, available in AD9834_ctrlReg
#define AD9834_FREQREG_MASK 0x3000u
#define AD9834_FREQREG_FULL 0x2000u
#define AD9834_FREQREG_MSB 0x1000u
#define AD9834_FREQREG_LSB 0x0000u

// freq/phase Register selection, available in AD9834_ctrlReg
#define AD9834_REGSEL_MASK 0x0C00u
#define AD9834_REGSEL_F1 0x0800u
#define AD9834_REGSEL_F0 0x0000u
#define AD9834_REGSEL_P1 0x0400u
#define AD9834_REGSEL_P0 0x0000u

// Register selection source, from hardware/software
#define AD9834_REGSEL_HW 0x0200u // hardware: DB9 = 1, software: DB9 = 0

// should be set before configuration, to disable output, available in AD9834_ctrlReg
#define AD9834_RESET 0x0100u

typedef enum _AD9834_IoutWaveType
{
  AD9834_IOff = 0,
  AD9834_Sine,
  AD9834_Tri,
} AD9834_IoutWaveType;

typedef enum _AD9834_SoutWaveType
{
  AD9834_SOff = 0,
  AD9834_Square,
  AD9834_Square2,
  AD9834_Square_Comp,
} AD9834_SoutWaveType;

typedef enum _AD9834_FreqConfMode
{
  AD9834_LSB = 0,
  AD9834_MSB,
  AD9834_Full,
} AD9834_FreqConfMode;

void AD9834_SetWaveType(AD9834_IoutWaveType IoutType, AD9834_SoutWaveType SoutType);
void AD9834_SelectReg(uint8_t freqRegID, uint8_t phaseRegID);
void AD9834_SetFreqConfMode(AD9834_FreqConfMode mode);

uint32_t AD9834_Freq2Reg(double freq, uint8_t regID);
double AD9834_GetActuralFreq(uint32_t regVal);
uint32_t AD9834_GetCurrentFreqReg(uint8_t regID);
void AD9834_SetFreq(double freq, uint8_t regID);
void AD9834_SetFreqMSB(double freq, uint8_t regID); // should be set to MSB mode First
void AD9834_SetFreqLSB(double freq, uint8_t regID); // should be set to LSB mode First

uint16_t AD9834_Phase2Reg(double phase, uint8_t regID);
double AD9834_GetActuralPhase(uint16_t regVal);
uint16_t AD9834_GetCurrentPhaseReg(uint8_t regID);
void AD9834_SetPhase(double phase, uint8_t regID);

void AD9834_Init(SPI_HandleTypeDef *hspi);
void AD9834_SendRaw(uint16_t data);
void AD9834_SetResetState(uint8_t isReset);
void AD9834_SetRegSelSrc(uint8_t isHw); // 1: Hardware 0: Software

#endif