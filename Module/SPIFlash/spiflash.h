#ifndef _SPIFLASH_H
#define _SPIFLASH_H

// for Winbond W25Xxx/W25Qxx Serial Flash Memory
// When reading, the clock freq should not be higher than 50MHz

#include "main.h"
#include "DELAY/delay.h"
#include "spi.h"

#define SPIFLASH_CS_GPIO GPIOB
#define SPIFLASH_CS_PIN GPIO_PIN_0
#define SPIFLASH_CS_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPIFLASH_CS_H() HAL_GPIO_WritePin(SPIFLASH_CS_GPIO, SPIFLASH_CS_PIN, GPIO_PIN_SET)
#define SPIFLASH_CS_L() HAL_GPIO_WritePin(SPIFLASH_CS_GPIO, SPIFLASH_CS_PIN, GPIO_PIN_RESET)

#define SPIFLASH_JEDECID 0x9F
#define SPIFLASH_UID 0x4B

#define SPIFLASH_PWRUP_ID 0xAB
#define SPIFLASH_PWRDN 0xB9

void SPIFlash_Init(SPI_HandleTypeDef *hspi);
uint8_t SPIFlash_GetDeviceID_PowerUp(void);
uint64_t SPIFlash_GetUID(void);
uint32_t SPIFlash_GetJEDECID(void);

#endif