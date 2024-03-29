#ifndef _ADF4351_H_
#define _ADF4351_H_
#include "main.h"

#define ADF4351_R4_PWR_N4DBM 0x00000000
#define ADF4351_R4_PWR_N1DBM 0x00000008
#define ADF4351_R4_PWR_2DBM 0x00000010
#define ADF4351_R4_PWR_5DBM 0x00000018
#define ADF4351_R4_PWR_MASK ADF4351_R4_PWR_5DBM

#define ADF4351_R4_AUXPWR_N4DBM 0x00000000
#define ADF4351_R4_AUXPWR_N1DBM 0x00000040
#define ADF4351_R4_AUXPWR_2DBM 0x00000080
#define ADF4351_R4_AUXPWR_5DBM 0x000000C0
#define ADF4351_R4_AUXPWR_MASK ADF4351_R4_AUXPWR_5DBM

typedef struct _ADF4351_CLKConfig
{
  double ref;
  // D, T and R are used to config Freq_PFD, storaged in R2;
  // Freq_PFD = Freq_REF * ((1 + D) / (R * (1 + T)))
  // Use T(R2_24) and CSR(R3_18) to enable cycle slip reduction
  uint8_t D, T;
  uint16_t R;
  // Div, INT, FRAC and MOD are used to config RF_out
  // Div_n is storaged in R4, which is related to VCO
  // 2 ^ Div_n = Div
  // (Div * RF_out = Freq_VCO, 2.2GHz < Freq_VCO < 4.4GHz)
  // MOD is storaged in R1, which is resolution to set resolution. Res = Freq_PFD / Div / MOD
  uint8_t Div_n;
  uint16_t MOD;
  // In the official ADF435x software, band select clock is less than or equal to 125k when generated
  // Use this divider to meet the requirement
  uint8_t BandClkDiv;
} ADF4351_CLKConfig;

void ADF4351_Init(void);
void ADF4351_Write(uint32_t val);
void ADF4351_Reg_Init(void);
// double ADF4351_SetFreq(float freq); //	(xx.x) M Hz

void ADF4351_SetRef(ADF4351_CLKConfig *config, double freqRef);
double ADF4351_SetPFD(ADF4351_CLKConfig *config, double freqPFD, uint8_t isDoubled, uint8_t is2Divided);
double ADF4351_GetPFD(ADF4351_CLKConfig *config);
uint8_t ADF4351_SetDiv(ADF4351_CLKConfig *config, uint8_t div);
uint8_t ADF4351_GetDiv(ADF4351_CLKConfig *config);
double ADF4351_SetResolution(ADF4351_CLKConfig *config, double resolution); // resolution in MHz
double ADF4351_GetResolution(ADF4351_CLKConfig *config);
double ADF4351_SetCLKConfig(ADF4351_CLKConfig *config, double freqRef, double freqPFD, uint8_t isDoubled, uint8_t is2Divided, uint8_t div, double resolution);
void ADF4351_WriteCLKConfig(ADF4351_CLKConfig *config);
uint8_t ADF4351_CalcDiv(double freqOut);

double ADF4351_SetFreq(ADF4351_CLKConfig *config, double freq);
void ADF4351_SetOutputPower(uint32_t pwr);
void ADF4351_SetAUXOutputPower(uint32_t pwr);
void ADF4351_SetRFout(uint8_t isEnabled);
void ADF4351_SetAUXRFout(uint8_t isEnabled);

#endif
