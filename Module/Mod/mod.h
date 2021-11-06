#ifndef _MOD_H
#define _MOD_H

#include "main.h"

void Mod_Tx_Init(TIM_TypeDef *outTIM, TIM_TypeDef *clkTIM, uint32_t Larr, uint32_t Harr);
void Mod_Tx_Start(void);
void Mod_Tx_SetValue(uint16_t data);
void Mod_Tx_Send(void);

void Mod_Rx_Read(uint8_t bit);

void OLED_Show4digit(uint8_t x, uint8_t y, int64_t val);
void Mod_Tx_Process(void);

#endif