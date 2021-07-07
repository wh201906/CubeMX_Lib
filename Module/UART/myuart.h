#ifndef _MYUART_H
#define _MYUART_H

#include "main.h"
#include "usart.h"
#include <stdio.h>

typedef struct _MyUARTHandle
{
  USART_TypeDef *USARTx;
  uint8_t *buffer;
  uint32_t bufferLen;
  uint32_t bufferPos;
  uint8_t isOverflow;
} MyUARTHandle;

void MyUART_SetOStream(USART_TypeDef *USARTx); // for printf()
void MyUART_Init(MyUARTHandle *handle, USART_TypeDef *USARTx, uint8_t *buffer, uint32_t bufferLen);

// return how many bytes are sent
void MyUART_WriteChar(MyUARTHandle *handle, uint8_t ch);
void MyUART_Write(MyUARTHandle *handle, uint8_t *str, uint16_t len);
uint32_t MyUART_WriteUntil(MyUARTHandle *handle, uint8_t *str, uint8_t endChar);
uint32_t MyUART_WriteUntilWithEnd(MyUARTHandle *handle, uint8_t *str, uint8_t endChar);
uint32_t MyUART_WriteStr(MyUARTHandle *handle, uint8_t *str);  // the \0 will NOT be sent
uint32_t MyUART_WriteLine(MyUARTHandle *handle, uint8_t *str); // the \0 of the string will not be sent, and the transimitted data ends with \r\n(no \0 at the end)

uint8_t MyUART_ReadChar(MyUARTHandle *handle);
uint8_t MyUART_PeekChar(MyUARTHandle *handle);
uint32_t MyUART_Read(MyUARTHandle *handle, uint8_t *str, uint16_t maxLen);

uint8_t MyUART_CanReadLine(MyUARTHandle *handle);
uint8_t MyUART_CanReadUntil(MyUARTHandle *handle, uint16_t endChar);
uint8_t MyUART_CanReadStr(MyUARTHandle *handle);

//for ReadStr(), ReadUntil() and ReadLine(), if the EndSequence doesn't match, they will return 0 and the bufferPos will not change.
uint32_t MyUART_ReadStr(MyUARTHandle *handle, uint8_t *str);
uint32_t MyUART_ReadUntil(MyUARTHandle *handle, uint8_t *str, uint16_t endChar);
uint32_t MyUART_ReadLine(MyUARTHandle *handle, uint8_t *str);
uint32_t MyUART_ReadAll(MyUARTHandle *handle, uint8_t *str);
void __MyUART_Shift(MyUARTHandle *handle, uint32_t len);
void __MyUART_DumpAll(MyUARTHandle *handle);
void MyUART_ClearBuffer(MyUARTHandle *handle);
void MyUART_IRQHandler(MyUARTHandle *handle);
#endif