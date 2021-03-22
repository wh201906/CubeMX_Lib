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
  SPIFlash_Buf[0] = SPIFLASH_PWRUP_ID;

  SPIFLASH_CS_L();
  SPIFLASH_T(4);
  SPIFLASH_R(1);
  SPIFLASH_CS_H();

  return SPIFlash_Buf[0];
}

uint64_t SPIFlash_GetUID(void)
{
  uint8_t i;
  uint64_t uid = 0u;
  SPIFlash_Buf[0] = SPIFLASH_UID;

  SPIFLASH_CS_L();
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
  SPIFlash_Buf[0] = SPIFLASH_JEDECID;

  SPIFLASH_CS_L();
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

// return the reg value
uint8_t SPIFlash_ReadStateReg(uint8_t regId)
{
  if (regId == 2)
    SPIFlash_Buf[0] = SPIFLASH_SR2_R;
  else if (regId == 3)
    SPIFlash_Buf[0] = SPIFLASH_SR3_R;
  else
    SPIFlash_Buf[0] = SPIFLASH_SR1_R;

  SPIFLASH_CS_L();
  SPIFLASH_T(1);
  SPIFLASH_R(1);
  SPIFLASH_CS_H();

  return SPIFlash_Buf[0];
}

// return whether the write is done
uint8_t SPIFlash_WriteStateReg(uint8_t regId, uint8_t val)
{
  if (regId == 1)
    SPIFlash_Buf[0] = SPIFLASH_SR1_W;
  else if (regId == 2)
    SPIFlash_Buf[0] = SPIFLASH_SR2_W;
  else if (regId == 3)
    SPIFlash_Buf[0] = SPIFLASH_SR3_W;
  else
    return 0;
  SPIFlash_Buf[1] = val;

  SPIFLASH_CS_L();
  SPIFLASH_T(2);
  SPIFLASH_CS_H();

#if SPIFLASH_WRITECHECK
  return SPIFlash_ReadStateReg(regId) == val;
#else
  return 1;
#endif
}

uint8_t SPIFlash_SetWriteEnabled(uint8_t isEnabled)
{
  if (isEnabled)
    SPIFlash_Buf[0] = SPIFLASH_WRITE_ENABLE;
  else
    SPIFlash_Buf[0] = SPIFLASH_WRITE_DISABLE;

  SPIFLASH_CS_L();
  SPIFLASH_T(1);
  SPIFLASH_CS_H();

#if SPIFLASH_WRITECHECK
  // cast to bool
  // !!(isEnabled) is COOL but not so explicit
  return ((SPIFlash_ReadStateReg(1) & 2u) ? 1 : 0) == (isEnabled ? 1 : 0);
#else
  return 1;
#endif
}

uint8_t SPIFlash_IsBusy(void)
{
  return (SPIFlash_ReadStateReg(1) & 1u);
}

// CS will not be pulled up there
void SPIFLASH_SendAddr(uint8_t rw, uint32_t addr)
{
  SPIFlash_Buf[0] = rw;
#if SPIFLASH_ADDR_4BYTE
  SPIFlash_Buf[1] = (addr >> 24) & 0xFF;
  SPIFlash_Buf[2] = (addr >> 16) & 0xFF;
  SPIFlash_Buf[3] = (addr >> 8) & 0xFF;
  SPIFlash_Buf[4] = (addr >> 0) & 0xFF;

  SPIFLASH_CS_L();
  SPIFLASH_T(5);
  SPIFLASH_CS_H();
#else
  SPIFlash_Buf[1] = (addr >> 16) & 0xFF;
  SPIFlash_Buf[2] = (addr >> 8) & 0xFF;
  SPIFlash_Buf[3] = (addr >> 0) & 0xFF;

  SPIFLASH_CS_L();
  SPIFLASH_T(4);
#endif
}

uint8_t SPIFlash_ReadByte(uint32_t addr)
{
  SPIFLASH_SendAddr(SPIFLASH_READ, addr);
  SPIFLASH_R(1);
  SPIFLASH_CS_H();

  return SPIFlash_Buf[0];
}

void SPIFlash_Erase(uint8_t type, uint32_t addr)
{
}

uint8_t SPIFlash_WriteByte(uint32_t addr, uint8_t val)
{
  if (SPIFlash_SetWriteEnabled(1))
    return 0;

  SPIFLASH_SendAddr(SPIFLASH_WRITE, addr);
  SPIFlash_Buf[0];
  SPIFLASH_T(1);
  SPIFLASH_CS_H();

#if SPIFLASH_WRITECHECK
  return SPIFlash_ReadByte(addr) == val;
#else
  return 1;
#endif
}