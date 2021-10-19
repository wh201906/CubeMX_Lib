#ifndef _CH423_H
#define _CH423_H

#include "main.h"
#include "I2C/softi2c.h"
// The CH423 doesn't use the typical I2C protocol

#define CH423_CONF 0x48
#define CH423_OCL_W 0x44
#define CH423_OCH_W 0x46
#define CH423_IO_W 0x60
#define CH423_IO_R 0x4D

#define CH423_IOPIN_IN 0
#define CH423_IOPIN_OUT (1 << 0)
#define CH423_IOPIN_MASK CH423_IOPIN_OUT

#define CH423_OCPIN_PUSHPULL 0
#define CH423_OCPIN_OPENDRAIN (1 << 4)
#define CH423_OCPIN_MASK CH423_OCPIN_OPENDRAIN

void CH423_Init(GPIO_TypeDef *SCL_GPIO, uint8_t SCL_PinID, GPIO_TypeDef *SDA_GPIO, uint8_t SDA_PinID);
void CH423_Write(uint8_t cmd, uint8_t data);
uint8_t CH423_Read(uint8_t cmd);
void CH423_WriteAll(uint32_t data);
void CH423_WriteOC(uint16_t data);
void CH423_WritePin(uint8_t id, uint8_t state);
uint8_t CH423_ReadPin(uint8_t id);

// CH423_IOPIN_IN / CH423_IOPIN_OUT
void CH423_SetIOPinMode(uint8_t mode);

// CH423_OCPIN_PUSHPULL / CH423_OCPIN_OPENDRAIN
void CHD23_SetOCPinMode(uint8_t mode);

#endif