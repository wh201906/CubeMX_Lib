#ifndef _MYUSART2_H
#define _MYUSART2_H
#include "main.h" 
#include "usart.h"
#include <stdio.h>

#define MYUSART2_MAX_LEN 100

#ifdef STM32H750xx
#define __MYUSART2_ENABLE_IT() ((USART2)->CR1 |= (USART_CR1_RXNEIE_RXFNEIE))
#endif
#ifdef STM32F407xx
#define __MYUSART2_ENABLE_IT() ((USART2)->CR1 |= ((UART_IT_RXNE) & UART_IT_MASK))
#endif

// used for bytes, lines and strings
// for Writexx() functions, the 0 represents false, and the function can be wrapped with single if()
// for ReadChar(), the result is the last fetched char in buffer;
// for the rest of Readxx(), the result is the number of chars written into str[];

void MyUSART2_Init(void);
void MyUSART2_WriteChar(uint8_t ch);
uint8_t MyUSART2_Write(uint8_t* str, uint16_t len);
uint8_t MyUSART2_WriteStr(uint8_t* str); // the \0 will BE sent;
uint8_t MyUSART2_WriteLine(uint8_t* str); // the \0 of the string will not be sent, and the transimitted data ends with \r\n(no \0 at the end)
uint8_t MyUSART2_WriteUntil(uint8_t* str,uint8_t endChar); // similar to the MyUSART2_WriteStr() with the customized EndChar
uint8_t MyUSART2_ReadChar(void);
uint8_t MyUSART2_PeekChar(void);
uint32_t MyUSART2_Read(uint8_t* str, uint16_t maxLen);

uint8_t MyUSART2_CanReadLine();
uint8_t MyUSART2_CanReadUntil(uint16_t endChar);
uint8_t MyUSART2_CanReadStr();

//for ReadStr(), ReadUntil() and ReadLine(), if the EndSequence doesn't match, they will return 0 and the bufferPos will not change.
uint32_t MyUSART2_ReadStr(uint8_t* str);
uint32_t MyUSART2_ReadUntil(uint8_t* str,uint16_t endChar);
uint32_t MyUSART2_ReadLine(uint8_t* str);
uint32_t MyUSART2_ReadAll(uint8_t* str);
void __MyUSART2_Shift(uint32_t len);
void __MyUSART2_DumpAll(void);
void MyUSART2_ClearBuffer();

void MyUSART2_IRQHandler(USART_TypeDef* source);

#endif

/*
Replacement for stm32fxxx_it.h/stm32hxxx_it.h

1.
add
#include "USART\myusart2.h" 
after
USER CODE BEGIN Includes

2.
add
MyUSART2_IRQHandler(USARTx);
in function
USARTx_IRQHandler()

Remember to enable usart interrupt in CubeMX
*/
