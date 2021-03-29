#include "testbench.h"

#define CHECKEDGE 10

uint8_t testBuf1[10000];
uint8_t testBuf2[10000];
uint32_t testRange[] = {
    2,
    1,
    0,
    3,
    14,
    15,
    16,
    17,
    30,
    31,
    32,
    33,
    126,
    127,
    128,
    129,
    254,
    255,
    256,
    257,
    510,
    511,
    512,
    513,
    1022,
    1023,
    1024,
    1025,
    4094,
    4095,
    4096,
    4097,
    8190,
    8191,
    8192,
    8193,
    9214,
    9215,
    9216,
    9217,
};
void SPIFlash_FunctionTest(void)
{
  uint32_t argLen = sizeof(testRange) / sizeof(testRange[0]);
  uint32_t i, j, k, l;
  uint32_t opStart, opLen, checkStart, checkLen;
  uint8_t isFailed;
  for (i = 0; i < argLen / 4; i++)
  {
    srand(SysTick->VAL);
    Delay_ticks(rand() % 100);
    opStart = testRange[i * 4 + 2];
    checkStart = opStart > CHECKEDGE ? opStart - CHECKEDGE : 0;
    opStart += 4096 * i;
    checkStart += 4096 * i;
    for (j = 0; j < argLen; j++)
    {
      isFailed = 0;
      srand(SysTick->VAL);
      opLen = testRange[j];
      checkLen = opLen + CHECKEDGE + (opStart - checkStart);
      for (k = 0; k < 3; k++)
      {
        Delay_ticks(rand() % 10);
        SPIFlash_Read(checkStart, testBuf1, checkLen);
        // for (l = 0; l < checkLen; l++)
        // {
        //   printf("%02x ", testBuf1[l]);
        // }
        // printf("\r\n");
        for (l = 0; l < opLen; l++)
          testBuf1[(opStart - checkStart) + l] = rand() & 0xFF;
        // for (l = 0; l < checkLen; l++)
        // {
        //   printf("%02x ", testBuf1[l]);
        // }
        // printf("\r\n");
        SPIFlash_Write(opStart, testBuf1 + (opStart - checkStart), opLen);
        SPIFlash_Read(checkStart, testBuf2, checkLen);
        for (l = 0; l < checkLen; l++)
        {
          // printf("%02x ", testBuf1[l]);
          if (testBuf1[l] != testBuf2[l])
            break;
        }
        // printf("\r\n");
        if (l < checkLen)
        {
          printf("#%d Failed\r\n", k + 1);
          isFailed = 1;
        }
      }
      if (isFailed)
        printf("Error at id=%u: opS=%u+%u, opL=%u, ckS=%u, ckL=%u\r\n", i * argLen + j, 4096 * i, opStart - 4096 * i, opLen, checkStart, checkLen);
    }
    printf("i=%u tested\r\n", i);
  }
}