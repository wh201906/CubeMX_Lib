#ifndef _AD9910_H
#define _AD9910_H

#include "main.h"
#include "DELAY/delay.h"

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long int

#define AD9910_MAS_REST(__ST__) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, (__ST__))
#define AD9910_PWR(__ST__) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, (__ST__))
#define AD9910_DRCTL(__ST__) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, (__ST__))
#define AD9910_DRHOLD(__ST__) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, (__ST__))

#define AD9910_SDIO(__ST__) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, (__ST__))
#define AD9910_SCLK(__ST__) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, (__ST__))
#define AD9910_CS(__ST__) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, (__ST__))

#define AD9910_PROFILE0(__ST__) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (__ST__))
#define AD9910_PROFILE1(__ST__) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, (__ST__))
#define AD9910_PROFILE2(__ST__) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, (__ST__))
#define AD9910_UP_DAT(__ST__) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, (__ST__))

void Init_ad9910(void);
void Freq_convert(ulong Freq);                                                            //写频率
void Amp_convert(unsigned int Amplitude);                                                 //写幅度
void Txramdata(int wave_num);                                                             //任意波形发生，参数：波形序号1-2
void Square_wave(uint Sample_interval);                                                   //方波;
void Sawtooth_wave(uint Sample_interval);                                                 //三角波
void SweepFre(ulong SweepMinFre, ulong SweepMaxFre, ulong SweepStepFre, ulong SweepTime); //扫频：Hz, Hz,Hz,us

void Set_RefCLK_OUT(uint8_t mode);

void Txcfr(void);

#endif