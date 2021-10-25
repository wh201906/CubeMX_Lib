#ifndef _AD5933_h_
#define _AD5933_h_

#include "main.h"

#define AD5933_ADDR 0x0D

#define AD5933_CMD_WRITEBLK 0xA0
#define AD5933_CMD_READBLK 0xA1
#define AD5933_CMD_SETADDR 0xB0

#define AD5933_REG_CTRL0 0x80
#define AD5933_REG_CTRL1 0x81

#define AD5933_REG_STARTFREQ0 0x82
#define AD5933_REG_STARTFREQ1 0x83
#define AD5933_REG_STARTFREQ2 0x84

#define AD5933_REG_STEPFREQ0 0x85
#define AD5933_REG_STEPFREQ1 0x86
#define AD5933_REG_STEPFREQ2 0x87

#define AD5933_REG_STEPNUM0 0x88
#define AD5933_REG_STEPNUM1 0x89

#define AD5933_REG_SETTINGTIME0 0x8A
#define AD5933_REG_SETTINGTIME1 0x8B

#define AD5933_REG_STATUS 0x8F

#define AD5933_REG_TEMP0 0x92
#define AD5933_REG_TEMP1 0x93

#define AD5933_REG_REAL0 0x94
#define AD5933_REG_REAL1 0x95

#define AD5933_REG_IMAG0 0x96
#define AD5933_REG_IMAG1 0x97

#define AD5933_SYS_Init (1) << 12
#define AD5933_Begin_Fre_Scan (2) << 12
#define AD5933_Fre_UP (3) << 12
#define AD5933_Fre_Rep (4) << 12
#define AD5933_Get_Temp (9) << 12
#define AD5933_Sleep (10) << 12
#define AD5933_Standby (11) << 12

#define AD5933_OUTPUT_2V (0) << 9
#define AD5933_OUTPUT_1V (3) << 9
#define AD5933_OUTPUT_400mV (2) << 9
#define AD5933_OUTPUT_200mV (1) << 9

#define AD5933_Gain_1 (1) << 8
#define AD5933_Gain_5 (0) << 8

#define AD5933_IN_MCLK (0) << 3
#define AD5933_OUT_MCLK (1) << 3

#define AD5933_Reset (1) << 4

//¶¨Òåº¯Êý
void AD5933_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID);
uint8_t AD5933_ReadReg(uint8_t reg, uint8_t *data);
uint8_t AD5933_WriteReg(uint8_t reg, uint8_t data);
float Scale_imp(uint8_t *SValue, uint8_t *IValue, uint8_t *NValue, uint8_t *CValue);
uint16_t AD5933_Tempter(void);
float Get_resistance(uint16_t num);
float AD5933_Sweep(float Fre_Begin, float Fre_UP, uint16_t UP_Num, uint16_t OUTPUT_Vatage, uint16_t Gain, uint16_t SWeep_Rep);
float AD5933_Get_Rs(void);
float AD5933_Get_Cap(float Fre);
float AD5933_Get_L(float Fre);
#endif
