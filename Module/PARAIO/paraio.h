#ifndef _PARAIO_H
#define _PARAIO_H

#include "main.h"
#include "DELAY/delay.h"
#include "tim.h"

#define PARAIO_DATAIN_GPIO GPIOD
#define PARAIO_DATAIN_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()

#define PARAIO_DATAIN2_GPIO GPIOE
#define PARAIO_DATAIN2_CLKEN() __HAL_RCC_GPIOE_CLK_ENABLE()

#define PARAIO_DATAOUT_GPIO GPIOD
#define PARAIO_DATAOUT_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()

void ParaIO_Init_GPIO_In(void);

void ParaIO_Init_DMA_In(uint8_t memUnitSize, uint8_t isCircular); // 8,16

void ParaIO_Init_In(TIM_HandleTypeDef *htim, uint8_t memUnitSize, uint8_t isCircular);

void ParaIO_Start_In(void *destAddr, uint32_t len);

uint8_t ParaIO_IsTranferCompleted_In(void);

void ParaIO_Init_GPIO_In2(void);

void ParaIO_Init_DMA_In2(uint8_t memUnitSize, uint8_t isCircular); // 8,16

void ParaIO_Init_In2(TIM_HandleTypeDef *htim, uint8_t memUnitSize, uint8_t isCircular);

void ParaIO_Start_In2(void *destAddr, uint32_t len);

uint8_t ParaIO_IsTranferCompleted_In2(void);

void ParaIO_Start_In_Sync(void *destAddr1, uint32_t len1, void *destAddr2, uint32_t len2);

void ParaIO_Init_GPIO_Out(void);

void ParaIO_Init_DMA_Out(uint8_t memUnitSize, uint8_t isCircular);

void ParaIO_Init_Out(TIM_HandleTypeDef *htim, uint8_t memUnitSize, uint8_t isCircular);

void ParaIO_Start_Out(void *srcAddr, uint32_t len);

uint8_t ParaIO_IsTranferCompleted_Out(void);

#endif