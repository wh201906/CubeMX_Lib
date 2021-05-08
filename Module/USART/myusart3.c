#include "USART/myusart3.h"

uint8_t MyUSART3_buffer[MYUSART3_MAX_LEN];
uint32_t MyUSART3_bufferPos = 0;
uint8_t MyUSART3_bufferOverflowFlag = 0;
UART_HandleTypeDef *MyUSART3_dummyHandle;

void MyUSART3_Init(UART_HandleTypeDef *huart)
{
  MyUSART3_dummyHandle = huart;
  __HAL_UART_ENABLE_IT(MyUSART3_dummyHandle, UART_IT_RXNE);
  ;
}

void MyUSART3_WriteChar(uint8_t ch)
{
  while (!__HAL_UART_GET_FLAG(MyUSART3_dummyHandle, UART_FLAG_TC))
    ;
#if defined(STM32H750xx) || defined(STM32L431xx)
  USART3->TDR = ch;
#endif
#if defined(STM32F407xx) || defined(STM32F103xB)
  USART3->DR = ch;
#endif
}

uint8_t MyUSART3_Write(uint8_t *str, uint16_t len)
{
  if (len > MYUSART3_MAX_LEN)
    return 0;
  for (uint32_t i = 0; i < len; i++)
  {
    MyUSART3_WriteChar(*(str + i));
  }
  return len;
}
uint8_t MyUSART3_WriteStr(uint8_t *str) // the \0 will BE sent;
{
  return MyUSART3_WriteUntil(str, '\0');
}

uint8_t MyUSART3_WriteLine(uint8_t *str) // the \0 of the string will not be sent, and the transimitted data ends with \r\n(no \0 at the end)
{
  uint32_t i;
  for (i = 0; i < MYUSART3_MAX_LEN; i++)
  {
    if (*(str + i) == '\0')
      break;
    MyUSART3_WriteChar(*(str + i));
  }
  MyUSART3_WriteChar('\r');
  MyUSART3_WriteChar('\n');
  return i + 2 < MYUSART3_MAX_LEN;
}

uint8_t MyUSART3_WriteUntil(uint8_t *str, uint8_t endChar) // similar to the MyUSART3_WriteStr() with the customized EndChar
{
  uint32_t i;
  for (i = 0; i < MYUSART3_MAX_LEN; i++)
  {
    MyUSART3_WriteChar(*(str + i));
    if (*(str + i) == endChar)
      break;
  }
  return i < MYUSART3_MAX_LEN;
}

uint8_t MyUSART3_ReadChar(void)
{
  if (MyUSART3_bufferPos != 0)
    MyUSART3_bufferPos--;
  else
    MyUSART3_buffer[0] = 0;
  return MyUSART3_buffer[MyUSART3_bufferPos];
}

uint8_t MyUSART3_PeekChar(void)
{
  if (MyUSART3_bufferPos == 0)
    return MyUSART3_buffer[MYUSART3_MAX_LEN - 1];
  return MyUSART3_buffer[MyUSART3_bufferPos - 1];
}

uint8_t MyUSART3_CanReadLine()
{
  uint32_t i;
  for (i = 1; i < MyUSART3_bufferPos; i++)
  {
    if (MyUSART3_buffer[i - 1] == '\r' && MyUSART3_buffer[i] == '\n')
      return 1;
  }
  return 0;
}

uint8_t MyUSART3_CanReadUntil(uint16_t endChar)
{
  uint32_t i;
  for (i = 0; i < MyUSART3_bufferPos; i++)
  {
    if (MyUSART3_buffer[i] == endChar)
      return 1;
  }
  return 0;
}

uint8_t MyUSART3_CanReadStr()
{
  return MyUSART3_CanReadUntil('\0');
}

uint32_t MyUSART3_Read(uint8_t *str, uint16_t maxLen)
{
  maxLen = maxLen < MyUSART3_bufferPos ? maxLen : MyUSART3_bufferPos;
  uint32_t i;
  for (i = 0; i < maxLen; i++)
  {
    *(str + i) = MyUSART3_buffer[i];
  }
  __MyUSART3_Shift(maxLen);
  return maxLen;
}

uint32_t MyUSART3_ReadStr(uint8_t *str)
{
  return MyUSART3_ReadUntil(str, '\0');
}

uint32_t MyUSART3_ReadLine(uint8_t *str)
{
  if (!MyUSART3_CanReadLine())
    return 0;
  uint32_t i;
  for (i = 0; i < MyUSART3_bufferPos; i++)
  {
    *(str + i) = MyUSART3_buffer[i];
    if (i > 0 && *(str + i - 1) == '\r' && *(str + i) == '\n')
    {
      i++;
      break;
    }
  }
  __MyUSART3_Shift(i);
  return i;
}

uint32_t MyUSART3_ReadUntil(uint8_t *str, uint16_t endChar)
{
  if (!MyUSART3_CanReadUntil(endChar))
    return 0;
  uint32_t i;
  for (i = 0; i < MyUSART3_bufferPos; i++)
  {
    *(str + i) = MyUSART3_buffer[i];
    if (*(str + i) == endChar)
    {
      i++;
      break;
    }
  }
  __MyUSART3_Shift(i);
  return i;
}

uint32_t MyUSART3_ReadAll(uint8_t *str)
{
  uint32_t i;
  for (i = 0; i < MyUSART3_bufferPos; i++)
  {
    *(str + i) = MyUSART3_buffer[i];
  }
  __MyUSART3_Shift(i);
  return i;
}

void __MyUSART3_Shift(uint32_t len)
{
  for (uint32_t i = 0; i < MYUSART3_MAX_LEN - len; i++)
  {
    MyUSART3_buffer[i] = MyUSART3_buffer[i + len];
  }
  MyUSART3_bufferPos -= len;
}

void __MyUSART3_DumpAll(void)
{
  if (MYUSART3_MAX_LEN > 100)
    return;
  MyUSART3_WriteChar(MyUSART3_bufferPos);
  for (uint32_t i = 0; i < MYUSART3_MAX_LEN; i++)
  {
    MyUSART3_WriteChar(MyUSART3_buffer[i]);
  }
}

void MyUSART3_ClearBuffer()
{
  MyUSART3_bufferPos = 0;
}

void MyUSART3_IRQHandler(USART_TypeDef *source)
{

  if (!__HAL_UART_GET_FLAG(MyUSART3_dummyHandle, UART_FLAG_RXNE))
    return;
#if defined(STM32H750xx) || defined(STM32L431xx)
  MyUSART3_buffer[MyUSART3_bufferPos++] = USART3->RDR;
#endif
#if defined(STM32F407xx) || defined(STM32F103xB)
  MyUSART3_buffer[MyUSART3_bufferPos++] = USART3->DR;
#endif
  if (MyUSART3_bufferPos == MYUSART3_MAX_LEN)
  {
    MyUSART3_bufferPos = 0;
    MyUSART3_bufferOverflowFlag = 1;
  }
}
