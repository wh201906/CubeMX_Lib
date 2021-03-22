#include "spiflash.h"

SPI_HandleTypeDef *SPIFlash_SPIHandler;
uint32_t SPIFlash_Timeout = 1000;

// Write 256Bit, Erase 4096Bit, cache before erase
uint8_t SPIFlash_Buf[12];

// Use SPIFlash_Buf for transmit and receive
#define SPIFLASH_R(__LEN__) HAL_SPI_Receive(SPIFlash_SPIHandler, SPIFlash_Buf, (__LEN__), SPIFlash_Timeout)
#define SPIFLASH_T(__LEN__) HAL_SPI_Transmit(SPIFlash_SPIHandler, SPIFlash_Buf, (__LEN__), SPIFlash_Timeout)

void SPIFlash_Init(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  SPIFLASH_CS_CLKEN();

  // pre-release
  SPIFLASH_CS_H();

  // The CS need to track the VCC due to the datasheet
  // So the CS is connected to VCC with a capacitor, and the mode should be Open-Drain
  // No pullup or pulldown there

  GPIO_InitStruct.Pin = SPIFLASH_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SPIFLASH_CS_GPIO, &GPIO_InitStruct);
  SPIFlash_SPIHandler = hspi;
}

// Release PowerDown then get device ID
uint8_t SPIFlash_GetDeviceID_PowerUp(void)
{
  SPIFLASH_CS_L();
  SPIFlash_Buf[0] = SPIFLASH_PWRUP_ID;
  SPIFLASH_T(4);
  SPIFLASH_R(1);
  SPIFLASH_CS_H();
  return SPIFlash_Buf[0];
}

uint64_t SPIFlash_GetUID(void)
{
  uint8_t i;
  uint64_t uid = 0u;
  SPIFLASH_CS_L();
  SPIFlash_Buf[0] = SPIFLASH_UID;
  SPIFLASH_T(5);
  SPIFLASH_R(8);
  SPIFLASH_CS_H();
  uid |= SPIFlash_Buf[0];
  for (i = 1; i < 8; i++)
  {
    uid <<= 8;
    uid |= SPIFlash_Buf[i];
  }
  return uid;
}

uint32_t SPIFlash_GetJEDECID(void)
{
  uint8_t i;
  uint32_t jedecId = 0u;
  SPIFLASH_CS_L();
  SPIFlash_Buf[0] = SPIFLASH_JEDECID;
  SPIFLASH_T(1);
  SPIFLASH_R(3);
  SPIFLASH_CS_H();
  jedecId |= SPIFlash_Buf[0];
  for (i = 1; i < 3; i++)
  {
    jedecId <<= 8;
    jedecId |= SPIFlash_Buf[i];
  }
  return jedecId;
}