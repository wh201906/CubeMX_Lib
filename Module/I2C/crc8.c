#include "crc8.h"

uint8_t crcRow[16];
uint8_t crcCol[16];

// treat crcTable[256] as crcTable[16,16]
// then crcTable[43] == crcTable[2*16+11] == crcTable[16,11]
// then crcTable[16,11] == crcTable[16,0] ^ crcTable[0,11]
//
// compress 256 bytes to 32 bytes, with configurable polynomial

void CRC8_Init(uint8_t polynomial)
{
  uint8_t i, j, crc;

  for (i = 0; i < 16; i++)
  {
    crc = i;
    for (j = 0; j < 8; j++)
      crc = (crc << 1) ^ ((crc & 0x80) ? polynomial : 0);
    crcRow[i] = crc;
  }
  for (i = 0; i < 16; i++)
  {
    crc = i << 4;
    for (j = 0; j < 8; j++)
      crc = (crc << 1) ^ ((crc & 0x80) ? polynomial : 0);
    crcCol[i] = crc;
  }
}

uint8_t CRC8_Calc(uint8_t initVal, uint8_t *data, uint32_t dataLen)
{
  uint8_t result = initVal;
  uint32_t i;
  for (i = 0; i < dataLen; i++)
    result = CRC8_CalcByte(result, *(data + i));
  return result;
}

inline uint8_t CRC8_CalcByte(uint8_t initVal, uint8_t data)
{
  data = data ^ initVal;
  return crcCol[data >> 4] ^ crcRow[data & 0x0F];
}
