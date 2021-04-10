#ifndef _MYUSART1_H
#define _MYUSART1_H
#include "main.h"
#include "usart.h"
#include <stdio.h>

#define MYUSART1_MAX_LEN 100

#if defined(STM32H750xx) || defined(STM32L431xx)
#define __MYUSART1_ENABLE_IT() ((USART1)->CR1 |= (1u << (UART_IT_RXNE & UART_IT_MASK)))
#endif
#if defined(STM32F407xx) || defined(STM32F103xB)
#define __MYUSART1_ENABLE_IT() ((USART1)->CR1 |= ((UART_IT_RXNE) & UART_IT_MASK))
#endif

// used for bytes, lines and strings
// for Writexx() functions, the 0 represents false, and the function can be wrapped with single if()
// for ReadChar(), the result is the last fetched char in buffer;
// for the rest of Readxx(), the result is the number of chars written into str[];

int fputc(int ch, FILE *f);
void MyUSART1_Init(void);
void MyUSART1_WriteChar(uint8_t ch);
uint8_t MyUSART1_Write(uint8_t *str, uint16_t len);
uint8_t MyUSART1_WriteStr(uint8_t *str);                    // the \0 will BE sent;
uint8_t MyUSART1_WriteLine(uint8_t *str);                   // the \0 of the string will not be sent, and the transimitted data ends with \r\n(no \0 at the end)
uint8_t MyUSART1_WriteUntil(uint8_t *str, uint8_t endChar); // similar to the MyUSART1_WriteStr() with the customized EndChar
uint8_t MyUSART1_ReadChar(void);
uint8_t MyUSART1_PeekChar(void);
uint32_t MyUSART1_Read(uint8_t *str, uint16_t maxLen);

uint8_t MyUSART1_CanReadLine();
uint8_t MyUSART1_CanReadUntil(uint16_t endChar);
uint8_t MyUSART1_CanReadStr();

//for ReadStr(), ReadUntil() and ReadLine(), if the EndSequence doesn't match, they will return 0 and the bufferPos will not change.
uint32_t MyUSART1_ReadStr(uint8_t *str);
uint32_t MyUSART1_ReadUntil(uint8_t *str, uint16_t endChar);
uint32_t MyUSART1_ReadLine(uint8_t *str);
uint32_t MyUSART1_ReadAll(uint8_t *str);
void __MyUSART1_Shift(uint32_t len);
void __MyUSART1_DumpAll(void);
void MyUSART1_ClearBuffer();

void MyUSART1_IRQHandler(USART_TypeDef *source);

#endif

/*
Patch for stm32fxxx_it.c/stm32hxxx_it.c

1.
add
#include "USART\myusart1.h" 
after
USER CODE BEGIN Includes

2.
add
MyUSART1_IRQHandler(USARTx);
in function
USARTx_IRQHandler()

Remember to enable usart interrupt in CubeMX
*/
