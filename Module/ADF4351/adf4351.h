#ifndef _ADF4351_H_
#define _ADF4351_H_
#include "main.h"

typedef struct _ADF4351_CLKConfig
{
  double ref, PFD, resolution, out;
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

} ADF4351_CLKConfig;

void ADF4351_Init(void);
void ADF4351_Write(uint32_t val);
void ADF4351_Reg_Init(void);
double ADF4351_SetFreq(float freq); //	(xx.x) M Hz

void ADF4351_SetRef(ADF4351_CLKConfig *config, double freqRef);
double ADF4351_SetPFD(ADF4351_CLKConfig *config, double freqPFD, uint8_t isDoubled, uint8_t is2Divided);
double ADF4351_GetPFD(ADF4351_CLKConfig *config);
uint8_t ADF4351_SetDiv(ADF4351_ClKConfig *config, uint8_t div);
uint8_t ADF4351_GetDiv(ADF4351_ClKConfig *config);
double ADF4351_SetResolution(ADF4351_ClKConfig *config, double resolution); // resolution in MHz
double ADF4351_GetResolution(ADF4351_ClKConfig *config);

#endif
