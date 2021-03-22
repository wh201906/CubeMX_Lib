#ifndef _SPIFLASH_H
#define _SPIFLASH_H

#include "main.h"
#include "DELAY/delay.h"
#include "spi.h"

#define SPIFLASH_CS_GPIO GPIOB
#define SPIFLASH_CS_PIN GPIO_PIN_0
#define SPIFLASH_CS_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPIFLASH_CS_H() HAL_GPIO_WritePin(SPIFLASH_CS_GPIO, SPIFLASH_CS_PIN, GPIO_PIN_SET)
#define SPIFLASH_CS_L() HAL_GPIO_WritePin(SPIFLASH_CS_GPIO, SPIFLASH_CS_PIN, GPIO_PIN_RESET)

void SPIFlash_Init(SPI_HandleTypeDef *hspi);

#endif