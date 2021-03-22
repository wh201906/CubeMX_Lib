#include "spiflash.h"

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
}