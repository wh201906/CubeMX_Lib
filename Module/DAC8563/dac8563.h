#ifndef _DAC8563_H_
#define _DAC8563_H_

#include "main.h"

#define DAC8563_CLR_GPIO GPIOD
#define DAC8563_CLR_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()
#define DAC8563_CLR_PIN GPIO_PIN_11

#define DAC8563_SYN_GPIO GPIOD
#define DAC8563_SYN_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()
#define DAC8563_SYN_PIN GPIO_PIN_12

#define DAC8563_SCK_GPIO GPIOD
#define DAC8563_SCK_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()
#define DAC8563_SCK_PIN GPIO_PIN_13

#define DAC8563_DIN_GPIO GPIOD
#define DAC8563_DIN_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()
#define DAC8563_DIN_PIN GPIO_PIN_14

#define DAC8563_LD_GPIO GPIOD
#define DAC8563_LD_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()
#define DAC8563_LD_PIN GPIO_PIN_15

#define DAC8563_CLR(__PINSTATE__) (DAC8563_CLR_GPIO->BSRR = (uint32_t)(DAC8563_CLR_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define DAC8563_SYN(__PINSTATE__) (DAC8563_SYN_GPIO->BSRR = (uint32_t)(DAC8563_SYN_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define DAC8563_SCK(__PINSTATE__) (DAC8563_SCK_GPIO->BSRR = (uint32_t)(DAC8563_SCK_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define DAC8563_DIN(__PINSTATE__) (DAC8563_DIN_GPIO->BSRR = (uint32_t)(DAC8563_DIN_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define DAC8563_LD(__PINSTATE__) (DAC8563_LD_GPIO->BSRR = (uint32_t)(DAC8563_LD_PIN) << ((__PINSTATE__) ? (0u) : (16u)))

#define CMD_SETA_UPDATEA 0x18    // 写入DAC-A输入寄存器并更新DAC-A +16位数据
#define CMD_SETB_UPDATEB 0x19    // 写入DAC-B输入寄存器并更新DAC-B +16位数据
#define CMD_UPDATE_ALL_DACS 0x0F // 更新所有DAC 后16位只需时钟即可

// Gain
#define CMD_GAIN 0x02        // default with internal Vref
#define DATA_GAIN_B2_A2 0x00 // 增益：DAC-B增益= 2，DAC-A增益= 2（默认为内部VREF）
#define DATA_GAIN_B2_A1 0x01 // 增益：DAC-B增益= 2，DAC-A增益= 1
#define DATA_GAIN_B1_A2 0x02 // 增益：DAC-B增益= 1，DAC-A增益= 2
#define DATA_GAIN_B1_A1 0x03 // 增益：DAC-B增益= 1，DAC-A增益= 1（上电默认值）

#define CMD_PWR_UP_A_B 0x20       // 设置DAC上电或掉电模式
#define DATA_PWR_UP_A 0x0001      // 上电DAC-A
#define DATA_PWR_UP_B 0x0002      // 上电 DAC-B
#define DATA_PWR_UP_A_B 0x0003    // 上电DAC-A和DAC-B
#define DATA_PWR_DOWN1_A 0x0041   // 关断DAC-A; 1kΩ至GND
#define DATA_PWR_DOWN1_B 0x0042   // 关断DAC-B; 1kΩ至GND
#define DATA_PWR_DOWN1_A_B 0x0043 // 关断DAC-A和DAC-B; 1kΩ至GND
#define DATA_PWR_DOWN2_A 0x0081   // 关断DAC-A; 100kΩ至GND
#define DATA_PWR_DOWN2_B 0x0082   // 关断DAC-B; 100kΩ至GND
#define DATA_PWR_DOWN2_A_B 0x0083 // 关断DAC-A和DAC-B; 100kΩ至GND
#define DATA_PWR_DOWN3_A 0x00C1   // 关断DAC-A; 高阻
#define DATA_PWR_DOWN3_B 0x00C2   // 关断DAC-B; 高阻
#define DATA_PWR_DOWN3_A_B 0x00C3 // 关断DAC-A和DAC-B;高阻

#define CMD_RESET_ALL_REG 0x28    // Software reset
#define DATA_RESET_ALL_REG 0x0001 // 复位所有寄存器并更新所有DAC（上电复位更新）
#define DATA_RESET_ADCAB 0x0000   //复位DAC-A和DAC-B输入寄存器并更新所有DAC

#define CMD_LDAC_DIS 0x30    // 设置LDAC寄存器
#define DATA_LDAC_AB 0x0000  //LDAC引脚有效用于DAC-B和DAC-A
#define DATA_LDAC_B 0x0001   //LDAC引脚有效用于DAC-B; 对DAC-A无效
#define DATA_LDAC_A 0x0002   //LDAC引脚对DAC-B无效; DAC-A有效
#define DATA_LDAC_NAB 0x0003 //LDAC引脚对DAC-B和DAC-A无效

#define CMD_INTERNAL_REF_DIS 0x38    // Disable internal reference and reset DACs to gain = 1
#define DATA_INTERNAL_REF_DIS 0x0000 // 禁用内部参考并将DAC复位为增益= 1

#define CMD_INTERNAL_REF_EN 0x38    // Enable Internal Reference & reset DACs to gain = 2
#define DATA_INTERNAL_REF_EN 0x0001 //启用内部参考和复位DAC以获得增益= 2

void DAC8563_Init(void);
void DAC8563_Write(uint8_t cmd, uint16_t data);
void DAC8563_SetOutputAB(uint16_t data_a, uint16_t data_b);
void DAC8563_SetOutput(uint8_t isChannelA, uint16_t data);

#endif
