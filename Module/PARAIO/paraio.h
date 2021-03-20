#ifndef _PARAIO_H
#define _PARAIO_H

#include "main.h"
#include "DELAY/delay.h"
#include "tim.h"

#define PARAIO_DATAIN_GPIO GPIOD
#define PARAIO_DATAIN_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()

void ParaIO_Init_GPIO_In(void);

void ParaIO_Init_DMA_In(void);

void ParaIO_Init_In(TIM_HandleTypeDef* htim);

void ParaIO_Start_In(void *destAddr,uint32_t len);

#endif 