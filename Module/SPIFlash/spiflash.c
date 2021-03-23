#include "spiflash.h"

SPI_HandleTypeDef *SPIFlash_SPIHandler;
uint32_t SPIFlash_Timeout = 1000;

// Write 256Bit, Erase 4096Bit, cache before erase
uint8_t SPIFlash_Data_Buf[4100]; // 4096
uint8_t SPIFlash_CMD_Buf[10];    // 8

// Use SPIFlash_Buf for transmit and receive
#define SPIFLASH_R(__LEN__) HAL_SPI_Receive(SPIFlash_SPIHandler, SPIFlash_CMD_Buf, (__LEN__), SPIFlash_Timeout)
#define SPIFLASH_T(__LEN__) HAL_SPI_Transmit(SPIFlash_SPIHandler, SPIFlash_CMD_Buf, (__LEN__), SPIFlash_Timeout)

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
  if (regId == 1)
    SPIFlash_Buf[0] = SPIFLASH_SR1_R;
  else if (regId == 2)
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

// CS will not be pulled down there
void SPIFLASH_SendAddr(uint8_t operation, uint32_t addr)
{
  SPIFlash_Buf[0] = operation;
#if SPIFLASH_ADDR_4BYTE
  SPIFlash_Buf[1] = (addr >> 24) & 0xFF;
  SPIFlash_Buf[2] = (addr >> 16) & 0xFF;
  SPIFlash_Buf[3] = (addr >> 8) & 0xFF;
  SPIFlash_Buf[4] = (addr >> 0) & 0xFF;

  SPIFLASH_CS_L();
  SPIFLASH_T(5);
#else
  SPIFlash_Buf[1] = (addr >> 16) & 0xFF;
  SPIFlash_Buf[2] = (addr >> 8) & 0xFF;
  SPIFlash_Buf[3] = (addr >> 0) & 0xFF;

  SPIFLASH_CS_L();
  SPIFLASH_T(4);
#endif
}

// The type can be: SPIFLASH_ERASE4K, SPIFLASH_ERASE32K, SPIFLASH_ERASE64K, SPIFLASH_ERASEALL
// The addr is the orignal address rather than the sector/32k/64k id
//
// It seems that the addr can be passed to Flash directly, but there are many implementations
// which make the alignment, so I keep it in my code.
void SPIFlash_Erase(uint8_t type, uint32_t addr)
{
  SPIFlash_SetWriteEnabled(1);

  if (type == SPIFLASH_ERASEALL)
  {
    SPIFlash_Buf[0] = type;

    SPIFLASH_CS_L();
    SPIFLASH_T(1);
    SPIFLASH_CS_H();

    while (SPIFlash_IsBusy())
      ;
    return;
  }

  // I don't know whether (addr >> 12; addr << 12;) will be optimized by compiler
  // but @HolmiumTS gives a better way
  // "x&((-1)<<n)"
  if (type == SPIFLASH_ERASE4K)
    addr &= (uint32_t)-1 << 12;
  else if (type == SPIFLASH_ERASE32K)
    addr &= (uint32_t)-1 << 15;
  else if (type == SPIFLASH_ERASE64K)
    addr &= (uint32_t)-1 << 16;
  else // treat as 4K
    addr &= (uint32_t)-1 << 12;

  SPIFLASH_SendAddr(type, addr);
  SPIFLASH_CS_H();

  while (SPIFlash_IsBusy())
    ;
  return;
}

uint8_t SPIFlash_ReadByte(uint32_t addr)
{
  SPIFLASH_SendAddr(SPIFLASH_READ, addr);
  SPIFLASH_R(1);
  SPIFLASH_CS_H();

  return SPIFlash_Buf[0];
}

// |---|---|---|---
// |---|---|-
// Just handle the tail
void SPIFlash_Read(uint32_t addr, uint8_t *data, uint32_t len)
{
  // the maxLen for HAL_SPI_Receive() is 16bit, so I need a loop
  uint32_t processedLen = 0, currLen = 0;
  SPIFLASH_SendAddr(SPIFLASH_READ, addr);

  while (processedLen < len)
  {
    currLen = len - processedLen;
    currLen = 65536 < currLen ? 65536 : currLen;
    HAL_SPI_Receive(SPIFlash_SPIHandler, data + processedLen, currLen, SPIFlash_Timeout);
    processedLen += currLen;
  }
  SPIFLASH_CS_H();
}

// |---|---|---|---
//    -|---|---|-
// Handle head and tail for 256Byte alignment
void SPIFlash_Program(uint32_t addr, uint8_t *data, uint32_t len)
{
  uint32_t processedLen = 0, currLen = 0;

  currLen = 256 - (addr % 256);
  currLen = len < currLen ? len : currLen;
  while (processedLen < len)
  {
    SPIFLASH_SendAddr(SPIFLASH_PROGRAM, addr + processedLen);
    HAL_SPI_Transmit(SPIFlash_SPIHandler, data + processedLen, currLen, SPIFlash_Timeout);
    SPIFLASH_CS_H();
    while (SPIFlash_IsBusy())
      ;

    processedLen + currLen;
    currLen = len - processedLen;
    currLen = 256 < currLen ? 256 : currLen;
  }
  // The assignment of currLen will be executed once more.
}

// |---|---|---|---
//    -|---|---|-
// Handle head and tail for 4kByte alignment
void SPIFlash_Write(uint32_t addr, uint8_t *data, uint32_t len)
{
  uint32_t processedLen = 0, currLen = 0;
  uint32_t offset = 0, secPos = 0;
  uint16_t i;

  currLen = 4096 - (addr % 4096);
  currLen = len < currLen ? len : currLen;
  while (processedLen < len)
  {
    secPos = addr + processedLen;
    offset = secPos & 0xFFF;
    secPos &= (uint32_t)-1 << 12;
    SPIFlash_Read(secPos, SPIFlash_Data_Buf, 4096);
    for (i = 0; i < currLen; i++)
    {
      if (SPIFlash_Data_Buf[offset + i] != 0xFF && SPIFlash_Data_Buf[offset + i] != data[processedLen + offset + i])
        break;
    }
    if (i < currLen)
    {
      SPIFlash_Erase(secPos);
      for (i = 0; i < currLen; i++)
        SPIFlash_Data_Buf[offset + i] = data[processedLen + offset + i];
      SPIFlash_Program(secPos, SPIFlash_Data_Buf, 4096);
    }
    else
      SPIFlash_Program(addr + processedLen, data + processedLen, currLen);

    processedLen += currLen;
    currLen = len - processedLen;
    currLen = 4096 < currLen ? 4096 : currLen;
  }
}

uint8_t SPIFlash_WriteByte(uint32_t addr, uint8_t val)
{
  if (SPIFlash_ReadByte(addr) == val)
    return 1;

  SPIFlash_Erase(SPIFLASH_ERASE4K, addr);

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