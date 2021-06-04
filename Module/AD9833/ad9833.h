#ifndef _AD9833_H
#define _AD9833_H

#include "main.h"
#include "spi.h"
#include "DELAY/delay.h"

#define AD9833_NSS_PIN GPIO_PIN_14
#define AD9833_NSS_GPIO GPIOB

#ifndef POW2_28
#define POW2_28 268435456u
#endif
#define AD9833_CLK 25000000u
#define AD9833_PHASE_DELTA 11.377777777777778

// the target Register prefix
#define AD9833_REGADDR_MASK 0xE000u
#define AD9833_REGADDR_CTRL 0x0000u
#define AD9833_REGADDR_FREQ0 0x4000u
#define AD9833_REGADDR_FREQ1 0x8000u
#define AD9833_REGADDR_PHASE0 0xC000u
#define AD9833_REGADDR_PHASE1 0xE000u

// the suffix of wave type and SLEEP1/SLEEP12, available in AD9833_ctrlReg
#define AD9833_WAVE_MASK 0x00FFu
#define AD9833_WAVE_OFF 0x00E0u     // Vout is set to MSB rather than DAC
#define AD9833_WAVE_SQUARE 0x0068u  // DAC disabled
#define AD9833_WAVE_SQUARE2 0x0060u // DAC disabled, freq/2
#define AD9833_WAVE_SINE 0x0000u
#define AD9833_WAVE_TRI 0x0002u

// freqReg configuring mode, available in AD9833_ctrlReg
#define AD9833_FREQREG_MASK 0x3000u
#define AD9833_FREQREG_FULL 0x2000u
#define AD9833_FREQREG_MSB 0x1000u
#define AD9833_FREQREG_LSB 0x0000u

// freq/phase Register selection, available in AD9833_ctrlReg
#define AD9833_REGSEL_MASK 0x0C00u
#define AD9833_REGSEL_F1 0x0800u
#define AD9833_REGSEL_P1 0x0400u

// should be set before configuration, to disable output, available in AD9833_ctrlReg
#define AD9833_RESET 0x0100u

typedef enum _AD9833_WaveType
{
  AD9833_Off = 0,
  AD9833_Square,
  AD9833_Square2,
  AD9833_Sine,
  AD9833_Tri,
} AD9833_WaveType;

typedef enum _AD9833_FreqConfMode
{
  AD9833_LSB = 0,
  AD9833_MSB,
  AD9833_Full,
} AD9833_FreqConfMode;

void AD9833_SetWaveType(AD9833_WaveType type);
void AD9833_SelectReg(uint8_t freqRegID, uint8_t phaseRegID);
void AD9833_SetFreqConfMode(AD9833_FreqConfMode mode);

uint32_t AD9833_Freq2Reg(double freq, uint8_t regID);
double AD9833_GetActuralFreq(uint32_t regVal);
uint32_t AD9833_GetCurrentFreqReg(uint8_t regID);
void AD9833_SetFreq(double freq, uint8_t regID);
void AD9833_SetFreqMSB(double freq, uint8_t regID); // should be set to MSB mode First
void AD9833_SetFreqLSB(double freq, uint8_t regID); // should be set to lSB mode First

uint16_t AD9833_Phase2Reg(double phase, uint8_t regID);
double AD9833_GetActuralPhase(uint16_t regVal);
uint16_t AD9833_GetCurrentPhaseReg(uint8_t regID);
void AD9833_SetPhase(double phase, uint8_t regID);

void AD9833_Init(SPI_HandleTypeDef *hspi);
void AD9833_SendRaw(uint16_t data);
void AD9833_SetResetState(uint8_t isReset);

#endif