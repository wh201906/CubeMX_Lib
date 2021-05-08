#include "USART/myusart2.h"

uint8_t MyUSART2_buffer[MYUSART2_MAX_LEN];
uint32_t MyUSART2_bufferPos = 0;
uint8_t MyUSART2_bufferOverflowFlag = 0;
UART_HandleTypeDef *MyUSART2_dummyHandle;

void MyUSART2_Init(UART_HandleTypeDef *huart)
{
  MyUSART2_dummyHandle = huart;
  __HAL_UART_ENABLE_IT(MyUSART2_dummyHandle, UART_IT_RXNE);
  ;
}

void MyUSART2_WriteChar(uint8_t ch)
{
  while (!__HAL_UART_GET_FLAG(MyUSART2_dummyHandle, UART_FLAG_TC))
    ;
#if defined(STM32H750xx) || defined(STM32L431xx)
  USART2->TDR = ch;
#endif
#if defined(STM32F407xx) || defined(STM32F103xB)
  USART2->DR = ch;
#endif
}

uint8_t MyUSART2_Write(uint8_t *str, uint16_t len)
{
  if (len > MYUSART2_MAX_LEN)
    return 0;
  for (uint32_t i = 0; i < len; i++)
  {
    MyUSART2_WriteChar(*(str + i));
  }
  return len;
}
uint8_t MyUSART2_WriteStr(uint8_t *str) // the \0 will BE sent;
{
  return MyUSART2_WriteUntil(str, '\0');
}

uint8_t MyUSART2_WriteLine(uint8_t *str) // the \0 of the string will not be sent, and the transimitted data ends with \r\n(no \0 at the end)
{
  uint32_t i;
  for (i = 0; i < MYUSART2_MAX_LEN; i++)
  {
    if (*(str + i) == '\0')
      break;
    MyUSART2_WriteChar(*(str + i));
  }
  MyUSART2_WriteChar('\r');
  MyUSART2_WriteChar('\n');
  return i + 2 < MYUSART2_MAX_LEN;
}

uint8_t MyUSART2_WriteUntil(uint8_t *str, uint8_t endChar) // similar to the MyUSART2_WriteStr() with the customized EndChar
{
  uint32_t i;
  for (i = 0; i < MYUSART2_MAX_LEN; i++)
  {
    MyUSART2_WriteChar(*(str + i));
    if (*(str + i) == endChar)
      break;
  }
  return i < MYUSART2_MAX_LEN;
}

uint8_t MyUSART2_ReadChar(void)
{
  if (MyUSART2_bufferPos != 0)
    MyUSART2_bufferPos--;
  else
    MyUSART2_buffer[0] = 0;
  return MyUSART2_buffer[MyUSART2_bufferPos];
}

uint8_t MyUSART2_PeekChar(void)
{
  if (MyUSART2_bufferPos == 0)
    return MyUSART2_buffer[MYUSART2_MAX_LEN - 1];
  return MyUSART2_buffer[MyUSART2_bufferPos - 1];
}

uint8_t MyUSART2_CanReadLine()
{
  uint32_t i;
  for (i = 1; i < MyUSART2_bufferPos; i++)
  {
    if (MyUSART2_buffer[i - 1] == '\r' && MyUSART2_buffer[i] == '\n')
      return 1;
  }
  return 0;
}

uint8_t MyUSART2_CanReadUntil(uint16_t endChar)
{
  uint32_t i;
  for (i = 0; i < MyUSART2_bufferPos; i++)
  {
    if (MyUSART2_buffer[i] == endChar)
      return 1;
  }
  return 0;
}

uint8_t MyUSART2_CanReadStr()
{
  return MyUSART2_CanReadUntil('\0');
}

uint32_t MyUSART2_Read(uint8_t *str, uint16_t maxLen)
{
  maxLen = maxLen < MyUSART2_bufferPos ? maxLen : MyUSART2_bufferPos;
  uint32_t i;
  for (i = 0; i < maxLen; i++)
  {
    *(str + i) = MyUSART2_buffer[i];
  }
  __MyUSART2_Shift(maxLen);
  return maxLen;
}

uint32_t MyUSART2_ReadStr(uint8_t *str)
{
  return MyUSART2_ReadUntil(str, '\0');
}

uint32_t MyUSART2_ReadLine(uint8_t *str)
{
  if (!MyUSART2_CanReadLine())
    return 0;
  uint32_t i;
  for (i = 0; i < MyUSART2_bufferPos; i++)
  {
    *(str + i) = MyUSART2_buffer[i];
    if (i > 0 && *(str + i - 1) == '\r' && *(str + i) == '\n')
    {
      i++;
      break;
    }
  }
  __MyUSART2_Shift(i);
  return i;
}

uint32_t MyUSART2_ReadUntil(uint8_t *str, uint16_t endChar)
{
  if (!MyUSART2_CanReadUntil(endChar))
    return 0;
  uint32_t i;
  for (i = 0; i < MyUSART2_bufferPos; i++)
  {
    *(str + i) = MyUSART2_buffer[i];
    if (*(str + i) == endChar)
    {
      i++;
      break;
    }
  }
  __MyUSART2_Shift(i);
  return i;
}

uint32_t MyUSART2_ReadAll(uint8_t *str)
{
  uint32_t i;
  for (i = 0; i < MyUSART2_bufferPos; i++)
  {
    *(str + i) = MyUSART2_buffer[i];
  }
  __MyUSART2_Shift(i);
  return i;
}

void __MyUSART2_Shift(uint32_t len)
{
  for (uint32_t i = 0; i < MYUSART2_MAX_LEN - len; i++)
  {
    MyUSART2_buffer[i] = MyUSART2_buffer[i + len];
  }
  MyUSART2_bufferPos -= len;
}

void __MyUSART2_DumpAll(void)
{
  if (MYUSART2_MAX_LEN > 100)
    return;
  MyUSART2_WriteChar(MyUSART2_bufferPos);
  for (uint32_t i = 0; i < MYUSART2_MAX_LEN; i++)
  {
    MyUSART2_WriteChar(MyUSART2_buffer[i]);
  }
}

void MyUSART2_ClearBuffer()
{
  MyUSART2_bufferPos = 0;
}

void MyUSART2_IRQHandler(USART_TypeDef *source)
{

  if (!__HAL_UART_GET_FLAG(MyUSART2_dummyHandle, UART_FLAG_RXNE))
    return;
#if defined(STM32H750xx) || defined(STM32L431xx)
  MyUSART2_buffer[MyUSART2_bufferPos++] = USART2->RDR;
#endif
#if defined(STM32F407xx) || defined(STM32F103xB)
  MyUSART2_buffer[MyUSART2_bufferPos++] = USART2->DR;
#endif
  if (MyUSART2_bufferPos == MYUSART2_MAX_LEN)
  {
    MyUSART2_bufferPos = 0;
    MyUSART2_bufferOverflowFlag = 1;
  }
}
