#ifndef _AD7606_H_
#define _AD7606_H_

// serial mode driver
// pull up PAR/SEL/BYTE SEL Pin,
// pull down DB15

#include "main.h"

#define AD7606_RST_GPIO GPIOB
#define AD7606_OS0_GPIO GPIOB
#define AD7606_OS1_GPIO GPIOB
#define AD7606_OS2_GPIO GPIOB
#define AD7606_CONVA_GPIO GPIOD
#define AD7606_CONVB_GPIO GPIOD
#define AD7606_BUSY_GPIO GPIOD
#define AD7606_SCK_GPIO GPIOD
#define AD7606_DIN_GPIO GPIOD
#define AD7606_CS_GPIO GPIOD

#define AD7606_RST_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define AD7606_OS0_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define AD7606_OS1_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define AD7606_OS2_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define AD7606_CONVA_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()
#define AD7606_CONVB_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()
#define AD7606_BUSY_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()
#define AD7606_SCK_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()
#define AD7606_DIN_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()
#define AD7606_CS_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()

#define AD7606_RST_PIN GPIO_PIN_6
#define AD7606_OS0_PIN GPIO_PIN_7
#define AD7606_OS1_PIN GPIO_PIN_8
#define AD7606_OS2_PIN GPIO_PIN_9
#define AD7606_CONVA_PIN GPIO_PIN_0
#define AD7606_CONVB_PIN GPIO_PIN_1
#define AD7606_BUSY_PIN GPIO_PIN_2
#define AD7606_SCK_PIN GPIO_PIN_3
#define AD7606_DIN_PIN GPIO_PIN_4
#define AD7606_CS_PIN GPIO_PIN_5

#define AD7606_RST(__PINSTATE__) (AD7606_RST_GPIO->BSRR = (uint32_t)(AD7606_RST_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define AD7606_OS0(__PINSTATE__) (AD7606_OS0_GPIO->BSRR = (uint32_t)(AD7606_OS0_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define AD7606_OS1(__PINSTATE__) (AD7606_OS1_GPIO->BSRR = (uint32_t)(AD7606_OS1_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define AD7606_OS2(__PINSTATE__) (AD7606_OS2_GPIO->BSRR = (uint32_t)(AD7606_OS2_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define AD7606_CONVA(__PINSTATE__) (AD7606_CONVA_GPIO->BSRR = (uint32_t)(AD7606_CONVA_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define AD7606_CONVB(__PINSTATE__) (AD7606_CONVB_GPIO->BSRR = (uint32_t)(AD7606_CONVB_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define AD7606_BUSY() (!!(AD7606_BUSY_GPIO->IDR & AD7606_BUSY_PIN))
#define AD7606_SCK(__PINSTATE__) (AD7606_SCK_GPIO->BSRR = (uint32_t)(AD7606_SCK_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define AD7606_DIN() (!!(AD7606_DIN_GPIO->IDR & AD7606_DIN_PIN))
#define AD7606_CS(__PINSTATE__) (AD7606_CS_GPIO->BSRR = (uint32_t)(AD7606_CS_PIN) << ((__PINSTATE__) ? (0u) : (16u)))

#define AD7606_OVERSAMPLE_NO 0
#define AD7606_OVERSAMPLE_2 1
#define AD7606_OVERSAMPLE_4 2
#define AD7606_OVERSAMPLE_8 3
#define AD7606_OVERSAMPLE_16 4
#define AD7606_OVERSAMPLE_32 5
#define AD7606_OVERSAMPLE_64 6

void AD7606_Init(void);
void AD7606_SetOversample(uint8_t oversample);
// channel id from 0~7
int16_t AD7606_ReadChannel(uint8_t channelId);
void AD7606_ReadAll(int16_t *result);
// note: the delay between the posedges of CONVA and CONVB cannot be longer than 0.5s
// which means I can't convert half of the channels individually
void AD7606_StartConvA(void);
void AD7606_StartConvB(void);
void AD7606_StartConvAll(void);
void AD7606_Reset(void);
#define AD7606_isBusy() AD7606_BUSY()

#endif