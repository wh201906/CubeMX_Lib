#ifndef _MYUSART_H
#define _MYUSART_H
#include "main.h" 
#include "usart.h"
#include <stdio.h>

#define MYUSART_MAX_LEN 40
#define MYUSART_CURR_USART USART1

#ifdef STM32H750xx
#define __MYUSART_ENABLE_IT() ((MYUSART_CURR_USART)->CR1 |= (USART_CR1_RXNEIE_RXFNEIE))
#endif
#ifdef STM32F407xx
#define __MYUSART_ENABLE_IT() ((MYUSART_CURR_USART)->CR1 |= ((UART_IT_RXNE) & UART_IT_MASK))
#endif

// used for bytes, lines and strings
// for Writexx() functions, the 0 represents false, and the function can be wrapped with single if()
// for ReadChar(), the result is the last fetched char in buffer;
// for the rest of Readxx(), the result is the number of chars written into str[];

int fputc(int ch, FILE *f);
void MyUSART_Init(void);
void MyUSART_WriteChar(uint8_t ch);
uint8_t MyUSART_Write(uint8_t* str, uint16_t len);
uint8_t MyUSART_WriteStr(uint8_t* str); // the \0 will BE sent;
uint8_t MyUSART_WriteLine(uint8_t* str); // the \0 of the string will not be sent, and the transimitted data ends with \r\n(no \0 at the end)
uint8_t MyUSART_WriteUntil(uint8_t* str,uint8_t endChar); // similar to the MyUSART_WriteStr() with the customized EndChar
uint8_t MyUSART_ReadChar(void);
uint8_t MyUSART_PeekChar(void);
uint32_t MyUSART_Read(uint8_t* str, uint16_t maxLen);

//for ReadStr(), ReadUntil() and ReadLine(), you can use if(buffer[MyUSART_Readxx(buffer)-1]==xx) to check the result 
uint32_t MyUSART_ReadStr(uint8_t* str);
uint32_t MyUSART_ReadUntil(uint8_t* str,uint16_t endChar);
uint32_t MyUSART_ReadLine(uint8_t* str);
uint32_t MyUSART_ReadAll(uint8_t* str);
void __MyUSART_Shift(uint32_t len);
void __MyUSART_DumpAll(void);
void MyUSART_ClearBuffer();

void MyUSART_IRQHandler(USART_TypeDef* source);

#endif

/*
Replacement for stm32fxxx_it.h

1.
add
#include "USART\myusart.h" 
after
USER CODE BEGIN Includes

2.
add
MyUSART_IRQHandler(USARTx);
in function
USARTx_IRQHandler()
*/
