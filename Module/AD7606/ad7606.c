#include "ad7606.h"
#include "DELAY/delay.h"
#include "UTIL/mygpio.h"

uint16_t AD7606_delayTicks;

void AD7606_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  AD7606_RST_CLKEN();
  AD7606_OS0_CLKEN();
  AD7606_OS1_CLKEN();
  AD7606_OS2_CLKEN();
  AD7606_CONVA_CLKEN();
  AD7606_CONVB_CLKEN();
  AD7606_BUSY_CLKEN();
  AD7606_SCK_CLKEN();
  AD7606_DIN_CLKEN();
  AD7606_CS_CLKEN();

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  MyGPIO_Init(AD7606_RST_GPIO, AD7606_RST_PIN, 0);
  MyGPIO_Init(AD7606_OS0_GPIO, AD7606_OS0_PIN, 0);
  MyGPIO_Init(AD7606_OS1_GPIO, AD7606_OS1_PIN, 0);
  MyGPIO_Init(AD7606_OS2_GPIO, AD7606_OS2_PIN, 0);
  MyGPIO_Init(AD7606_CONVA_GPIO, AD7606_CONVA_PIN, 1);
  MyGPIO_Init(AD7606_CONVB_GPIO, AD7606_CONVB_PIN, 1);
  MyGPIO_Init(AD7606_SCK_GPIO, AD7606_SCK_PIN, 1);
  MyGPIO_Init(AD7606_CS_GPIO, AD7606_CS_PIN, 1);

  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  MyGPIO_Init(AD7606_BUSY_GPIO, AD7606_BUSY_PIN, 1);
  MyGPIO_Init(AD7606_DIN_GPIO, AD7606_DIN_PIN, 1);

  AD7606_delayTicks = Delay_GetSYSFreq() * 0.000000025 + 1.0; // 25ns

  AD7606_Reset();
}

// OS bits are latched at the negedge of BUSY,
// To apply the change, start a conversion and wait the BUSY becoming LOW
void AD7606_SetOversample(uint8_t oversample)
{
  if (oversample > AD7606_OVERSAMPLE_64)
    oversample = AD7606_OVERSAMPLE_64;
  // AD7606_xx() will convert pinstate into 0/1
  AD7606_OS0(oversample & 0x1);
  AD7606_OS1(oversample & 0x2);
  AD7606_OS2(oversample & 0x4);
}

int16_t AD7606_GetVal(uint8_t channelId)
{
  uint16_t val;
  uint8_t i;
  channelId *= 16; // skip channelId * 16 clocks
  AD7606_CS(0);
  for (i = 0; i < channelId; i++)
  {
    AD7606_SCK(0);
    Delay_ticks(AD7606_delayTicks);
    AD7606_SCK(1);
    Delay_ticks(AD7606_delayTicks);
  }
  for (i = 0; i < 16; i++)
  {
    AD7606_SCK(0);
    Delay_ticks(AD7606_delayTicks);
    val |= AD7606_DIN(); // read before change
    AD7606_SCK(1);
    val <<= 1;
    Delay_ticks(AD7606_delayTicks);
  }
  AD7606_CS(1);
  return (int16_t)val;
}

void AD7606_StartConvA(void)
{
  AD7606_CONVA(0);
  Delay_ticks(AD7606_delayTicks); // minimum pulse
  AD7606_CONVA(1);
  Delay_ticks(AD7606_delayTicks * 2); // wait posedge of BUSY
}

void AD7606_StartConvB(void)
{
  AD7606_CONVB(0);
  Delay_ticks(AD7606_delayTicks); // minimum pulse
  AD7606_CONVB(1);
  Delay_ticks(AD7606_delayTicks * 2); // wait posedge of BUSY
}

void AD7606_StartConvAll(void)
{
  AD7606_CONVA(0);
  AD7606_CONVB(0);
  Delay_ticks(AD7606_delayTicks); // minimum pulse
  AD7606_CONVA(1);
  AD7606_CONVB(1);
  Delay_ticks(AD7606_delayTicks * 2); // wait posedge of BUSY
}

void AD7606_Reset()
{
  AD7606_RST(1);
  Delay_ticks(AD7606_delayTicks * 2); // minimum pulse
  AD7606_RST(0);
}