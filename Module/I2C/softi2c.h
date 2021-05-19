#ifndef _SOFTI2C1_H
#define _SOFTI2C1_H

#include "main.h"
#include "DELAY/delay.h"
#include "UTIL/gpio_compat.h"

typedef struct _SoftI2C_Port
{
  GPIO_TypeDef *SCL_GPIO, *SDA_GPIO;
  uint8_t SCL_PinID, SDA_PinID;
  uint16_t SCL_Pin, SDA_Pin;
  uint16_t delayTicks, halfTicks;
  uint8_t addrLen;
} SoftI2C_Port;

// ****** configuration start ******
#define SOFTI2C_RETRYTIMES 3
// ****** configuration end ******

#define SOFTI2C_SCL(__PORT__, __PINSTATE__) ((__PORT__)->SCL_GPIO->BSRR = (uint32_t)((__PORT__)->SCL_Pin) << ((__PINSTATE__) ? (0u) : (16u)))
#define SOFTI2C_SDA(__PORT__, __PINSTATE__) ((__PORT__)->SDA_GPIO->BSRR = (uint32_t)((__PORT__)->SDA_Pin) << ((__PINSTATE__) ? (0u) : (16u)))
#define SOFTI2C_READSDA(__PORT__) (((__PORT__)->SDA_GPIO->IDR >> (__PORT__)->SDA_PinID) & 1u)

// Release SDA to read data
#define SOFTI2C_SDA_IN(__PORT__) SOFTI2C_SDA((__PORT__), 1)

#define SI2C_ACK 0
#define SI2C_NACK 1

#define SI2C_WRITE 0
#define SI2C_READ 1

#define SI2C_ADDR_7b 0
#define SI2C_ADDR_10b 1

// For most of the cases
void SoftI2C_SetPort(SoftI2C_Port *port, GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID);
void SoftI2C_Init(SoftI2C_Port *port, uint32_t speed, uint8_t addrLen);
uint8_t SoftI2C_SendAddr(SoftI2C_Port *port, uint16_t addr, uint8_t RorW);
uint8_t SoftI2C_Read(SoftI2C_Port *port, uint16_t deviceAddr, uint8_t memAddr, uint8_t *dataBuf, uint32_t dataSize);
uint8_t SoftI2C_Write(SoftI2C_Port *port, uint16_t deviceAddr, uint8_t memAddr, uint8_t *dataBuf, uint32_t dataSize);

// Low Layer
void SoftI2C_Start(SoftI2C_Port *port);
void SoftI2C_RepStart(SoftI2C_Port *port);
void SoftI2C_Stop(SoftI2C_Port *port);
void SoftI2C_SendACK(SoftI2C_Port *port, uint8_t ACK);
uint8_t SoftI2C_WaitACK(SoftI2C_Port *port);
void SoftI2C_SendByte(SoftI2C_Port *port, uint8_t byte);
uint8_t SoftI2C_ReadByte(SoftI2C_Port *port);
uint8_t SoftI2C_SendByte_ACK(SoftI2C_Port *port, uint8_t byte, uint8_t handleACK);
uint8_t SoftI2C_ReadByte_ACK(SoftI2C_Port *port, uint8_t ACK);

#endif