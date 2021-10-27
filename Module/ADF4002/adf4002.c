#include "adf4002.h"
#include "DELAY/delay.h"
#include "UTIL/mygpio.h"

uint16_t ADF4002_delayTicks = 168;

#define ADF4002_LE(__ST__) (HAL_GPIO_WritePin(ADF4002_LE_GPIO, ADF4002_LE_PIN, (__ST__)))
#define ADF4002_SCK(__ST__) (HAL_GPIO_WritePin(ADF4002_SCK_GPIO, ADF4002_SCK_PIN, (__ST__)))
#define ADF4002_DATA(__ST__) (HAL_GPIO_WritePin(ADF4002_DATA_GPIO, ADF4002_DATA_PIN, (__ST__)))
#define ADF4002_Delay() Delay_us(ADF4002_delayTicks)

void ADF4002_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  ADF4002_LE_CLKEN();
  ADF4002_SCK_CLKEN();
  ADF4002_DATA_CLKEN();

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  MyGPIO_Init(ADF4002_LE_GPIO, ADF4002_LE_PIN, 1);
  MyGPIO_Init(ADF4002_SCK_GPIO, ADF4002_SCK_PIN, 0);
  MyGPIO_Init(ADF4002_DATA_GPIO, ADF4002_DATA_PIN, 0);

  ADF4002_delayTicks = Delay_GetSYSFreq() * 0.000000025 + 1.0; // 10ns
}

void ADF4002_Write(uint8_t regID, uint32_t data) // regID is useless so far
{
  uint8_t len = 24;
  data <<= 8;
  ADF4002_LE(0);
  ADF4002_Delay();
  while (len--)
  {
    ADF4002_SCK(0);
    ADF4002_DATA(!!(data & 0x80000000));
    ADF4002_Delay();
    ADF4002_SCK(1);
    ADF4002_Delay();
    data <<= 1;
  }
  ADF4002_LE(1);
}

void Test(uint32_t val)
{
  ADF4002_Write(2, 0x0D8082);
  ADF4002_Write(0, 0x001F40); // 40MHz ref, PFD at 20k
  ADF4002_Write(1, val << 8 | 0x01);
}