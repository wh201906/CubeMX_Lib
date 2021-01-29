#ifndef _SOFTI2C1_H
#define _SOFTI2C1_H

#include "main.h"
#include "DELAY/delay.h"
#include "softi2c_common.h"

// ****** configuration start ******
#define SOFTI2C1_SCL_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define SOFTI2C1_SDA_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()

#define SOFTI2C1_SCL_GPIO GPIOB
#define SOFTI2C1_SDA_GPIO GPIOB

#define SOFTI2C1_SCL_PINID 8
#define SOFTI2C1_SDA_PINID 9

#define SOFTI2C1_RETRYTIMES 3
// ****** configuration end ******

#define SOFTI2C1_SCL_PIN ((uint16_t)1u << SOFTI2C1_SCL_PINID)
#define SOFTI2C1_SDA_PIN ((uint16_t)1u << SOFTI2C1_SDA_PINID)

#define SOFTI2C1_SCL(__PINSTATE__) (SOFTI2C1_SCL_GPIO->BSRR = (uint32_t)SOFTI2C1_SCL_PIN << ((__PINSTATE__) ? (0u) : (16u)))
#define SOFTI2C1_SDA(__PINSTATE__) (SOFTI2C1_SDA_GPIO->BSRR = (uint32_t)SOFTI2C1_SDA_PIN << ((__PINSTATE__) ? (0u) : (16u)))
#define SOFTI2C1_READSDA() ((SOFTI2C1_SDA_GPIO->IDR >> SOFTI2C1_SDA_PINID) & 1u)

#define SOFTI2C1_SDA_IN() {SOFTI2C1_SDA_GPIO->MODER &= ~(3 << (SOFTI2C1_SDA_PINID * 2u)); SOFTI2C1_SDA_GPIO->MODER |= 0 << (SOFTI2C1_SDA_PINID * 2u);}
#define SOFTI2C1_SDA_OUT() {SOFTI2C1_SDA_GPIO->MODER &= ~(3 << (SOFTI2C1_SDA_PINID * 2u)); SOFTI2C1_SDA_GPIO->MODER |= 1 << (SOFTI2C1_SDA_PINID * 2u);}

// For most of the cases
void SoftI2C1_Init(uint32_t speed);
uint8_t SoftI2C1_SendAddr(uint16_t addr, uint8_t addrLen, uint8_t RorW);
uint8_t SoftI2C1_Read(uint16_t deviceAddr, uint8_t deviceAddrLen, uint8_t memAddr, uint8_t *dataBuf, uint32_t dataSize);
uint8_t SoftI2C1_Write(uint16_t deviceAddr, uint8_t deviceAddrLen, uint8_t memAddr, uint8_t *dataBuf, uint32_t dataSize);

// Low Layer
void SoftI2C1_Start(void);
void SoftI2C1_Stop(void);
void SoftI2C1_SendACK(uint8_t ACK);
uint8_t SoftI2C1_WaitACK(void);
void SoftI2C1_SendByte(uint8_t byte);
uint8_t SoftI2C1_ReadByte(void);
uint8_t SoftI2C1_SendByte_ACK(uint8_t byte, uint8_t handleACK);
uint8_t SoftI2C1_ReadByte_ACK(uint8_t ACK);

#endif