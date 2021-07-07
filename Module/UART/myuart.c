#include "myuart.h"

USART_TypeDef *MyUART_OStreamUSARTx = USART1;

FILE __stdout;

void MyUART_Init(MyUARTHandle *handle, USART_TypeDef *USARTx, uint8_t *buffer, uint32_t bufferLen)
{
  handle->USARTx = USARTx;
  handle->buffer = buffer;
  handle->bufferLen = bufferLen;
  handle->bufferPos = 0;
  handle->isOverflow = 0;
  LL_USART_EnableIT_RXNE(USARTx);
}

int fputc(int ch, FILE *f)
{
  while (!LL_USART_IsActiveFlag_TC(MyUART_OStreamUSARTx))
    ;
  LL_USART_TransmitData8(MyUART_OStreamUSARTx, ch);
  return ch;
}

// for printf()
void MyUART_SetOStream(USART_TypeDef *USARTx)
{
  MyUART_OStreamUSARTx = USARTx;
}

void MyUART_WriteChar(MyUARTHandle *handle, uint8_t ch)
{
  while (!LL_USART_IsActiveFlag_TC(handle->USARTx))
    ;
  LL_USART_TransmitData8(handle->USARTx, ch);
}

void MyUART_Write(MyUARTHandle *handle, uint8_t *str, uint16_t len)
{
  uint32_t i;
  for (i = 0; i < len; i++)
    MyUART_WriteChar(handle, str[i]);
}

// endChar will NOT be sent
// Warning:
// 0x00 will be ignored, make sure the str has the endChar
uint32_t MyUART_WriteUntil(MyUARTHandle *handle, uint8_t *str, uint8_t endChar)
{
  uint32_t i = 0;
  while (str[i] != endChar)
    MyUART_WriteChar(handle, str[i++]);
  return i;
}

// endChar will be sent
// Warning:
// 0x00 will be ignored, make sure the str has the endChar
uint32_t MyUART_WriteUntilWithEnd(MyUARTHandle *handle, uint8_t *str, uint8_t endChar)
{
  uint32_t len;
  len = MyUART_WriteUntil(handle, str, endChar);
  MyUART_WriteChar(handle, endChar);
  return (len + 1);
}

// the \0 will NOT be sent
// to send a string with \0, use MyUART_WriteUntilWithEnd(handle, str, '\0')
uint32_t MyUART_WriteStr(MyUARTHandle *handle, uint8_t *str)
{
  return MyUART_WriteUntil(handle, str, '\0');
}

// the \0 of the string will not be sent, and the transimitted data ends with \r\n(no \0 at the end)
uint32_t MyUART_WriteLine(MyUARTHandle *handle, uint8_t *str)
{
  uint32_t len;
  len = MyUART_WriteStr(handle, str);
  MyUART_WriteChar(handle, '\r');
  MyUART_WriteChar(handle, '\n');
  return (len + 2);
}

void MyUART_IRQHandler(MyUARTHandle *handle)
{
  LL_USART_ClearFlag_RXNE(handle->USARTx);
  if (handle->bufferPos >= handle->bufferLen)
  {
    handle->bufferPos = 0;
    handle->isOverflow = 1;
    return;
  }
  handle->buffer[handle->bufferPos++] = LL_USART_ReceiveData8(handle->USARTx);
}