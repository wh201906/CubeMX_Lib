#include "si4703.h"
#include "I2C/softi2c.h"
#include "DELAY/delay.h"
#include "UTIL/mygpio.h"

SoftI2C_Port SI4703_port;

// How could a device not have register address?????
// So strange

// According to the AN230 Si4700/01/02/03 Programming Guide,
// Read: 0x0A, 0x0B, ..., 0x0F, 0x00, ..., 0x09
// Write: 0x02, 0x03, ..., 0x0F, 0x00, 0x01
uint16_t SI4703_regs[16];

#define SI4703_RST_PORT GPIOE
#define SI4703_RST_CLKEN() __HAL_RCC_GPIOE_CLK_ENABLE()
#define SI4703_RST_PIN GPIO_PIN_3
#define SI4703_RST(__PINSTATE__) (SI4703_RST_PORT->BSRR = (uint32_t)(SI4703_RST_PIN) << ((__PINSTATE__) ? (0u) : (16u)))

uint32_t SI4703_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  SI4703_RST_CLKEN();
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  MyGPIO_Init(SI4703_RST_PORT, SI4703_RST_PIN, 1);

  SoftI2C_SetPort(&SI4703_port, SCL_GPIO, SCL_PinID, SDA_GPIO, SDA_PinID);
  SoftI2C_Init(&SI4703_port, 400000, SI2C_ADDR_7b);
  SI4703_Reset();

  SI4703_GetReg(SI4703_READALL); // read all
  SI4703_regs[SI4703_TEST1] |= SI4703_TEST1_XOSCEN;
  SI4703_regs[SI4703_CONF1] |= SI4703_CONF1_STCIEN;
  SI4703_SetReg(SI4703_TEST1); // contain CONF1
  Delay_ms(510);               // wait oscillator
  SI4703_GetReg(SI4703_READALL);
  SI4703_regs[SI4703_PWR] = SI4703_PWR_DMUTE | SI4703_PWR_ENABLE;
  SI4703_regs[SI4703_CONF1] |= SI4703_CONF1_RDS; //Enable RDS

  SI4703_regs[SI4703_CONF1] |= SI4703_CONF1_DE_50;     //50kHz Europe setup
  SI4703_regs[SI4703_CONF2] |= SI4703_CONF2_SPACE_100; //100kHz channel spacing for Europe

  SI4703_regs[SI4703_CONF2] &= ~SI4703_CONF2_VOLUME_MASK; //Clear volume bits
  SI4703_regs[SI4703_CONF2] |= 0x0001;                    //Set volume
  SI4703_SetReg(SI4703_CONF2);

  Delay_ms(120);          // wait power up
  return SI4703_ReadID(); // the chipID will be changed after power up, containing the firmware version
}

uint8_t SI4703_GetReg(uint8_t reg)
{
  uint8_t bH, bL;
  uint8_t i;

  if (reg > 0x0F)
    return 0;
  if (reg < 0x0A)
    reg += 7;
  else
    reg -= 9;

  SoftI2C_Start(&SI4703_port);
  if (!SoftI2C_SendAddr(&SI4703_port, SI4703_ADDR, SI2C_READ))
    return 0;

  for (i = 1; i < reg; i++)
  {
    bH = SoftI2C_ReadByte_ACK(&SI4703_port, SI2C_ACK);
    bL = SoftI2C_ReadByte_ACK(&SI4703_port, SI2C_ACK);
    SI4703_regs[i + (i >= 7 ? -7 : +9)] = ((uint16_t)bH << 8) | bL;
  }
  bH = SoftI2C_ReadByte_ACK(&SI4703_port, SI2C_ACK);
  bL = SoftI2C_ReadByte_ACK(&SI4703_port, SI2C_NACK);
  SI4703_regs[i + (i >= 7 ? -7 : +9)] = ((uint16_t)bH << 8) | bL;
  SoftI2C_Stop(&SI4703_port);

  // according to the AN230, the last acknowledge can be ACK or NACK
  // however, if the last acknowledge is ACK and the I2C is configured as Open-Drain,
  // the MCU cannot make a proper STOP signal(SDA is forced LOW by Si4703)
  // In all, last NACK is necessary.

  return 1;
}

uint8_t SI4703_SetReg(uint8_t reg)
{
  uint8_t bH, bL;
  uint8_t i;

  if (reg <= 0x01 || reg >= 0x0A) // read only registers
    return 0;
  reg -= 1;

  SoftI2C_Start(&SI4703_port);
  if (!SoftI2C_SendAddr(&SI4703_port, SI4703_ADDR, SI2C_WRITE))
    return 0;
  for (i = 1; i <= reg; i++)
  {
    if (!SoftI2C_SendByte_ACK(&SI4703_port, (SI4703_regs[i + 1] & 0xFF00) >> 8, SI2C_ACK))
      return 0;
    if (!SoftI2C_SendByte_ACK(&SI4703_port, SI4703_regs[i + 1] & 0x00FF, SI2C_ACK))
      return 0;
  }
  SoftI2C_Stop(&SI4703_port);

  return 1;
}

uint32_t SI4703_ReadID(void)
{
  if (!SI4703_GetReg(SI4703_CHIPID))
    return 0;
  return ((uint32_t)SI4703_regs[SI4703_DEVICEID] << 16 | SI4703_regs[SI4703_CHIPID]);
}

void SI4703_Reset(void)
{
  HAL_GPIO_WritePin(SI4703_port.SDA_GPIO, SI4703_port.SDA_Pin, 0);
  SI4703_RST(0);
  Delay_ms(1);
  SI4703_RST(1);
  Delay_us(1);
  HAL_GPIO_WritePin(SI4703_port.SDA_GPIO, SI4703_port.SDA_Pin, 1);
  Delay_us(1);
}

uint8_t SI4703_SetFreq(double freq)
{
  uint8_t timeout;
  uint16_t CH;
  CH = (freq - 87.5) / 0.1 + 0.5;

  SI4703_GetReg(SI4703_CH);
  SI4703_regs[SI4703_CH] &= ~SI4703_CH_CHAN_MASK;
  SI4703_regs[SI4703_CH] |= CH & SI4703_CH_CHAN_MASK;
  SI4703_regs[SI4703_CH] |= SI4703_CH_TUNE;
  SI4703_SetReg(SI4703_CH);

  for (timeout = 0; timeout < 100; timeout++)
  {
    // Note:
    // When using the integrated internal oscillator, the Si4702/03-C19 seek/tune performance may be affected by data activity on the SDIO(SDA) bus.
    // So, if the integrated oscillator is used, don't polling STATUS register for the tune status, use interrupt on GPIO2 or a fixed delay instead.
    SI4703_GetReg(SI4703_STATUS);
    if (SI4703_regs[SI4703_STATUS] & SI4703_STATUS_STC)
      break;
    Delay_ms(1);
  }
  if (timeout >= 100)
    return 0;

  SI4703_GetReg(SI4703_CH);
  SI4703_regs[SI4703_CH] &= ~SI4703_CH_TUNE; //Clear the tune after a tune has completed
  SI4703_SetReg(SI4703_CH);
  return 1;
}

void SI4703_SetVolume(uint8_t volume) // 0~30 or 0~15, depending on the firmware
{
  uint8_t ext = 0;
  if ((SI4703_regs[SI4703_CHIPID] & SI4703_CHIPID_FIRMWARE_MASK) < 16) // firmware doesn't support Extended Volume Range.
  {
    if (volume > 15)
      volume = 15;
  }
  else
  {
    if (volume > 15)
    {
      if (volume > 30)
        volume = 30;
      volume++;
    }
    else
    {
      ext = 1;
    }
  }

  SI4703_GetReg(SI4703_CONF3);
  SI4703_regs[SI4703_CONF2] &= ~SI4703_CONF2_VOLUME_MASK;
  SI4703_regs[SI4703_CONF2] |= volume & SI4703_CONF2_VOLUME_MASK;
  SI4703_regs[SI4703_CONF3] &= ~SI4703_CONF3_VOLEXT;
  SI4703_regs[SI4703_CONF3] |= ext ? SI4703_CONF3_VOLEXT : 0;
  SI4703_SetReg(SI4703_CONF3);
}

uint8_t SI4703_ReadRSSI(void)
{
  if (!SI4703_GetReg(SI4703_STATUS))
    return 0;
  return (SI4703_regs[SI4703_STATUS] & SI4703_STATUS_RSSI_MASK);
}