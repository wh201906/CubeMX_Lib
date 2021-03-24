#ifndef _SPIFLASH_H
#define _SPIFLASH_H

// for Winbond W25Xxx/W25Qxx Serial Flash Memory
// When reading(03h), the clock freq should not be higher than 50MHz
// When fast reading(0Bh), the max clock freq is between 104MHz~133MHz, depended on the VCC。

#include "main.h"
#include "DELAY/delay.h"
#include "spi.h"

#define SPIFLASH_CS_GPIO GPIOB
#define SPIFLASH_CS_PIN GPIO_PIN_0
#define SPIFLASH_CS_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()

// Write check works for WriteStateReg(), SetWriteEnabled() and WriteByte()
#define SPIFLASH_WRITECHECK 1
#define SPIFLASH_ADDR_4BYTE 0

#define SPIFLASH_CS_H() HAL_GPIO_WritePin(SPIFLASH_CS_GPIO, SPIFLASH_CS_PIN, GPIO_PIN_SET)
#define SPIFLASH_CS_L() HAL_GPIO_WritePin(SPIFLASH_CS_GPIO, SPIFLASH_CS_PIN, GPIO_PIN_RESET)

#define SPIFLASH_JEDECID 0x9F
#define SPIFLASH_UID 0x4B
#define SPIFLASH_PWRUP_ID 0xAB
#define SPIFLASH_PWRDN 0xB9

#define SPIFLASH_SR1_R 0x05
#define SPIFLASH_SR1_W 0x01
#define SPIFLASH_SR2_R 0x35
#define SPIFLASH_SR2_W 0x31
#define SPIFLASH_SR3_R 0x15
#define SPIFLASH_SR3_W 0x11

#define SPIFLASH_WRITE_ENABLE 0x06
#define SPIFLASH_WRITE_DISABLE 0x04

// Fast read is not implemented there. This instruction is useful when the SPI clock is over 50MHz
#define SPIFLASH_READ 0x03
#define SPIFLASH_READ_FAST 0x0B
#define SPIFLASH_PROGRAM 0x02
#define SPIFLASH_ERASE4K 0x20
#define SPIFLASH_ERASE32K 0x52
#define SPIFLASH_ERASE64K 0xD8
#define SPIFLASH_ERASEALL 0xC7

void SPIFlash_Init(SPI_HandleTypeDef *hspi);

void SPIFlash_PowerDown(void);
uint8_t SPIFlash_GetDeviceID_PowerUp(void);
uint64_t SPIFlash_GetUID(void);
uint32_t SPIFlash_GetJEDECID(void);

uint8_t SPIFlash_ReadStateReg(uint8_t regId);
uint8_t SPIFlash_WriteStateReg(uint8_t regId, uint8_t val);
uint8_t SPIFlash_SetWriteEnabled(uint8_t isEnabled);
uint8_t SPIFlash_IsBusy(void);

void SPIFlash_Erase(uint8_t type, uint32_t addr);
uint8_t SPIFlash_ReadByte(uint32_t addr);
void SPIFlash_Read(uint32_t addr, uint8_t *data, uint32_t len);
void SPIFlash_Program(uint32_t addr, uint8_t *data, uint32_t len);
void SPIFlash_Write(uint32_t addr, uint8_t *data, uint32_t len);
uint8_t SPIFlash_WriteByte(uint32_t addr, uint8_t val);

#endif
