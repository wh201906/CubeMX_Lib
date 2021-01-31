#ifndef _SOFTI2C2_H
#define _SOFTI2C2_H

#include "main.h"
#include "DELAY/delay.h"
#include "softi2c_common.h"

// ****** configuration start ******
#define SOFTI2C2_SCL_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define SOFTI2C2_SDA_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()

#define SOFTI2C2_SCL_GPIO GPIOB
#define SOFTI2C2_SDA_GPIO GPIOB

#define SOFTI2C2_SCL_PINID 6
#define SOFTI2C2_SDA_PINID 7

#define SOFTI2C2_RETRYTIMES 3
// ****** configuration end ******

#define SOFTI2C2_SCL_PIN ((uint16_t)1u << SOFTI2C2_SCL_PINID)
#define SOFTI2C2_SDA_PIN ((uint16_t)1u << SOFTI2C2_SDA_PINID)

#define SOFTI2C2_SCL(__PINSTATE__) (SOFTI2C2_SCL_GPIO->BSRR = (uint32_t)SOFTI2C2_SCL_PIN << ((__PINSTATE__) ? (0u) : (16u)))
#define SOFTI2C2_SDA(__PINSTATE__) (SOFTI2C2_SDA_GPIO->BSRR = (uint32_t)SOFTI2C2_SDA_PIN << ((__PINSTATE__) ? (0u) : (16u)))
#define SOFTI2C2_READSDA() ((SOFTI2C2_SDA_GPIO->IDR >> SOFTI2C2_SDA_PINID) & 1u)

#define SOFTI2C2_SDA_IN() {SOFTI2C2_SDA_GPIO->MODER &= ~(3 << (SOFTI2C2_SDA_PINID * 2u)); SOFTI2C2_SDA_GPIO->MODER |= 0 << (SOFTI2C2_SDA_PINID * 2u);}
#define SOFTI2C2_SDA_OUT() {SOFTI2C2_SDA_GPIO->MODER &= ~(3 << (SOFTI2C2_SDA_PINID * 2u)); SOFTI2C2_SDA_GPIO->MODER |= 1 << (SOFTI2C2_SDA_PINID * 2u);}

// For most of the cases
void SoftI2C2_Init(uint32_t speed);
uint8_t SoftI2C2_SendAddr(uint16_t addr, uint8_t addrLen, uint8_t RorW);
uint8_t SoftI2C2_Read(uint16_t deviceAddr, uint8_t deviceAddrLen, uint8_t memAddr, uint8_t *dataBuf, uint32_t dataSize);
uint8_t SoftI2C2_Write(uint16_t deviceAddr, uint8_t deviceAddrLen, uint8_t memAddr, uint8_t *dataBuf, uint32_t dataSize);

// Low Layer
void SoftI2C2_Start(void);
void SoftI2C2_RepStart(void);
void SoftI2C2_Stop(void);
void SoftI2C2_SendACK(uint8_t ACK);
uint8_t SoftI2C2_WaitACK(void);
void SoftI2C2_SendByte(uint8_t byte);
uint8_t SoftI2C2_ReadByte(void);
uint8_t SoftI2C2_SendByte_ACK(uint8_t byte, uint8_t handleACK);
uint8_t SoftI2C2_ReadByte_ACK(uint8_t ACK);

#endif