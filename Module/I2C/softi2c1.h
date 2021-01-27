#ifndef _SOFTI2C1_H
#define _SOFTI2C1_H

#include "main.h"
#include "DELAY/delay.h"

// ****** configuration start ******
#define SOFTI2C1_SCL_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define SOFTI2C1_SDA_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()

#define SOFTI2C1_SCL_GPIO GPIOB
#define SOFTI2C1_SDA_GPIO GPIOB

#define SOFTI2C1_SCL_PINID 8
#define SOFTI2C1_SDA_PINID 9
// ****** configuration end ******

#define SOFTI2C1_SCL_PIN ((uint16_t)1u << SOFTI2C1_SCL_PINID)
#define SOFTI2C1_SDA_PIN ((uint16_t)1u << SOFTI2C1_SDA_PINID)

#define SOFTI2C1_SCL(__PINSTATE__) (SOFTI2C1_SCL_GPIO->BSRR = (uint32_t)SOFTI2C1_SCL_PIN << ((__PINSTATE__) ? (0u) : (16u)))
#define SOFTI2C1_SDA(__PINSTATE__) (SOFTI2C1_SDA_GPIO->BSRR = (uint32_t)SOFTI2C1_SDA_PIN << ((__PINSTATE__) ? (0u) : (16u)))
#define SOFTI2C1_READSDA() ((SOFTI2C1_SDA_GPIO->IDR >> SOFTI2C1_SDA_PINID) & 1u)

#define SOFTI2C1_SDA_IN() {SOFTI2C1_SDA_GPIO->MODER &= ~(3 << (SOFTI2C1_SDA_PINID * 2u)); SOFTI2C1_SDA_GPIO->MODER |= 0 << (SOFTI2C1_SDA_PINID * 2u);}
#define SOFTI2C1_SDA_OUT() {SOFTI2C1_SDA_GPIO->MODER &= ~(3 << (SOFTI2C1_SDA_PINID * 2u)); SOFTI2C1_SDA_GPIO->MODER |= 1 << (SOFTI2C1_SDA_PINID * 2u);}


void SoftI2C1_Init(uint32_t speed);
void SoftI2C1_Start(void);
void SoftI2C1_Stop(void);
void SoftI2C1_SendACK(uint8_t isACK);
uint8_t SoftI2C1_WaitACK(void);
void SoftI2C1_SendByte(uint8_t byte);
uint8_t SoftI2C1_ReadByte(uint8_t sendACK);

void SoftI2C1_Master_Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
void SoftI2C1_Master_Receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

void SoftI2C1_Slave_Transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout);
void SoftI2C1_Slave_Receive(uint8_t *pData, uint16_t Size, uint32_t Timeout);

void SoftI2C1_Write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
void SoftI2C1_Read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

void SoftI2C1_IsDeviceReady(uint16_t DevAddress, uint32_t Trials, uint32_t Timeout);

void SoftI2C1_Start(void);

#endif