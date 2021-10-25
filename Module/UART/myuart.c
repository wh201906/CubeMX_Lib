#include "myuart.h"

USART_TypeDef *MyUART_OStreamUSARTx = USART1;

FILE __stdout;

void MyUART_Init(MyUARTHandle *handle, USART_TypeDef *USARTx, uint8_t *buffer, uint32_t bufferLen)
{
  handle->USARTx = USARTx;
  handle->buffer = buffer;
  handle->bufferLen = bufferLen;
  handle->headPos = 0;
  handle->tailPos = 0;
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

uint8_t MyUART_ReadChar(MyUARTHandle *handle)
{
  uint8_t ch;
  if (MyUART_IsEmpty(handle))
    return 0;
  else
  {
    ch = handle->buffer[handle->headPos++];
    handle->headPos %= handle->bufferLen;
    return ch;
  }
}

uint8_t MyUART_PeekChar(MyUARTHandle *handle)
{
  if (MyUART_IsEmpty(handle))
    return 0;
  else
    return handle->buffer[handle->headPos];
}

uint32_t MyUART_Read(MyUARTHandle *handle, uint8_t *str, uint32_t maxLen)
{
  uint32_t i;
  for (i = 0; i < maxLen && !MyUART_IsEmpty(handle); i++)
  {
    str[i] = handle->buffer[handle->headPos++];
    handle->headPos %= handle->bufferLen;
  }
  return i;
}

// return: position of \r + 1 (0~len-1 -> 1~len, nonzero) or 0
uint8_t MyUART_CanReadLine(MyUARTHandle *handle)
{
  uint32_t i;
  if (handle->buffer[handle->headPos] == '\r' && handle->buffer[MyUART_HeadNext(handle)] == '\n')
    return 1;
  // handle '\n' first to make sure both '\r' and '\n' are valid
  for (; i != handle->tailPos; i++)
  {
    i %= handle->bufferLen;
    if (handle->buffer[i] == '\n' && (i == 0 && handle->buffer[handle->bufferLen - 1] == '\r')) // \nXXX...XX\r
      return 1;
    if (handle->buffer[i] == '\n' && handle->buffer[i - 1] == '\r')
      return 1;
  }
  return 0;
}

// return: position of endChar + 1 (0~len-1 -> 1~len, nonzero) or 0
uint8_t MyUART_CanReadUntil(MyUARTHandle *handle, uint16_t endChar)
{
  uint32_t i;
  for (i = handle->headPos; i != handle->tailPos; i++)
  {
    i %= handle->bufferLen;
    if (handle->buffer[i] == endChar)
      return 1;
  }
  return 0;
}

uint8_t MyUART_CanReadStr(MyUARTHandle *handle)
{
  return MyUART_CanReadUntil(handle, '\0');
}

uint32_t MyUART_ReadStr(MyUARTHandle *handle, uint8_t *str)
{
  return MyUART_ReadUntil(handle, str, '\0');
}

uint32_t MyUART_ReadAll(MyUARTHandle *handle, uint8_t *str)
{
  return MyUART_Read(handle, str, 0xFFFFFFFF);
}

uint32_t MyUART_ReadUntil(MyUARTHandle *handle, uint8_t *str, uint16_t endChar)
{
  uint32_t i;
  if (!MyUART_CanReadUntil(handle, endChar))
    return 0;
  for (i = 0; !MyUART_IsEmpty(handle); i++)
  {
    if (handle->buffer[handle->headPos] == endChar)
    {
      handle->headPos++;
      handle->headPos %= handle->bufferLen;
      break;
    }
    str[i] = handle->buffer[handle->headPos++];
    handle->headPos %= handle->bufferLen;
  }
  return i;
}

uint32_t MyUART_ReadUntilWithEnd(MyUARTHandle *handle, uint8_t *str, uint16_t endChar)
{
  uint32_t result;
  result = MyUART_ReadUntil(handle, str, endChar);
  if (result == 0)
    return 0;
  str[result++] = endChar;
  return result;
}

uint32_t MyUART_ReadUntilWithZero(MyUARTHandle *handle, uint8_t *str, uint16_t endChar)
{
  uint32_t result;
  result = MyUART_ReadUntil(handle, str, endChar);
  if (result == 0)
    return 0;
  str[result++] = '\0';
  return result;
}

uint32_t MyUART_ReadLine(MyUARTHandle *handle, uint8_t *str)
{
  uint32_t i;
  if (!MyUART_CanReadLine(handle))
    return 0;
  for (i = 0; !MyUART_IsEmpty(handle); i++)
  {
    // if CanReadLine returns 1, the '\n' is a valid char
    if (handle->buffer[handle->headPos] == '\r' && handle->buffer[MyUART_HeadNext(handle)] == '\n')
    {
      handle->headPos += 2;
      handle->headPos %= handle->bufferLen;
      break;
    }
    str[i] = handle->buffer[handle->headPos++];
    handle->headPos %= handle->bufferLen;
  }
  return i;
}

void MyUART_ClearBuffer(MyUARTHandle *handle)
{
  handle->tailPos = 0;
  handle->headPos = 0;
}

void MyUART_IRQHandler(MyUARTHandle *handle)
{
  uint32_t newTail;
  #if !defined(STM32H743xx) // on STM32H7, reading DR register will clear the RX flag
    LL_USART_ClearFlag_RXNE(handle->USARTx);
  #endif
  newTail = MyUART_TailNext(handle);
  if (newTail == handle->headPos)
  {
    handle->isOverflow = 1;
    return;
  }
  handle->buffer[handle->tailPos] = LL_USART_ReceiveData8(handle->USARTx);
  handle->tailPos = newTail;
}