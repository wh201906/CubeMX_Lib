#ifndef _ADS1115_H
#define _ADS1115_H

#include "main.h"
#include "I2C/softi2c.h"
#include "DELAY/delay.h"

#define ADS1115_ADDR 0x48 // ADDR connected to GND
// #define ADS1115_ADDR 0x49 // ADDR connected to VDD
// #define ADS1115_ADDR 0x4A // ADDR connected to SDA(need special operation, see datasheet)
// #define ADS1115_ADDR 0x4B // ADDR connected to SCL

// Address Pointer Register
#define ADS1115_CONV 0x00    // Conversion register
#define ADS1115_CONF 0x01    // Config register
#define ADS1115_THRE_LO 0x02 // low threshold register(for comparator)
#define ADS1115_THRE_HI 0x03 // high threshold register(for comparator)

#define ADS1115_CONF_OS 0x8000 // operational status/single-short start

#define ADS1115_CONF_MUX_01 0x0000 // Mux: AIN0 - AIN1 (default)
#define ADS1115_CONF_MUX_03 0x1000 // Mux: AIN0 - AIN3
#define ADS1115_CONF_MUX_13 0x2000 // Mux: AIN1 - AIN3
#define ADS1115_CONF_MUX_23 0x3000 // Mux: AIN2 - AIN3
#define ADS1115_CONF_MUX_0G 0x4000 // Mux: AIN0 - GND
#define ADS1115_CONF_MUX_1G 0x5000 // Mux: AIN1 - GND
#define ADS1115_CONF_MUX_2G 0x6000 // Mux: AIN2 - GND
#define ADS1115_CONF_MUX_3G 0x7000 // Mux: AIN3 - GND

#define ADS1115_CONF_PGA_6144 0x0000 // PGA: +-6.144V
#define ADS1115_CONF_PGA_4096 0x0200 // PGA: +-4.096V
#define ADS1115_CONF_PGA_2048 0x0400 // PGA: +-2.048V (default)
#define ADS1115_CONF_PGA_1024 0x0600 // PGA: +-1.024V
#define ADS1115_CONF_PGA_512 0x0800  // PGA: +-0.512V
#define ADS1115_CONF_PGA_256 0x0A00  // PGA: +-0.256V (CONF[11:9]=PGA[2:0]=101/110/111)

#define ADS1115_CONF_MODE_CONTINUOUS 0x0000 // Continuous-conversion mode
#define ADS1115_CONF_MODE_SINGLE 0x0100     // Single-shot mode or power-down state (default)

#define ADS1115_CONF_DR_8 0x0000   // DataRate: 8 SPS
#define ADS1115_CONF_DR_16 0x0020  // DataRate: 16 SPS
#define ADS1115_CONF_DR_32 0x0040  // DataRate: 32 SPS
#define ADS1115_CONF_DR_64 0x0060  // DataRate: 64 SPS
#define ADS1115_CONF_DR_128 0x0080 // DataRate: 128 SPS (default)
#define ADS1115_CONF_DR_250 0x00A0 // DataRate: 250 SPS
#define ADS1115_CONF_DR_475 0x00C0 // DataRate: 475 SPS
#define ADS1115_CONF_DR_860 0x00E0 // DataRate: 860 SPS

#define ADS1115_CONF_COMPMODE_TRADITION 0x0000 // (default)
#define ADS1115_CONF_COMPMODE_WINDOW 0x0010

#define ADS1115_CONF_COMPPOL_ACL 0X0000 // Active low (default)
#define ADS1115_CONF_COMPPOL_ACH 0x0008 // Active high

#define ADS1115_CONF_COMPLATCH_OFF 0x0000 // (default)
#define ADS1115_CONF_COMPLATCH_ON 0x0004

#define ADS1115_CONF_COMPQUEUE_1 0x0000   // Assert after one conversion
#define ADS1115_CONF_COMPQUEUE_2 0x0001   // Assert after two conversions
#define ADS1115_CONF_COMPQUEUE_4 0x0002   // Assert after four conversions
#define ADS1115_CONF_COMPQUEUE_OFF 0x0003 // Disable Comparator and set ALERT/RDY pin to high-Z (default)

void ADS1115_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID);
uint8_t ADS1115_ReadReg(uint8_t reg, uint16_t *data);
uint8_t ADS1115_WriteReg(uint8_t reg, uint16_t data);

#endif