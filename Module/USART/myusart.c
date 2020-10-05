#include "USART/myusart.h"

uint8_t MyUSART_buffer[MYUSART_MAX_LEN];
uint32_t MyUSART_bufferPos=0;

int fputc(int ch, FILE *f)
{ 	
#ifdef STM32H750xx
	while((MYUSART_CURR_USART->ISR&0X40)==0);
	MYUSART_CURR_USART->TDR=(uint8_t)ch;
#endif
#ifdef STM32F407xx
    while((MYUSART_CURR_USART->SR&0X40)==0);
	MYUSART_CURR_USART->DR = (uint8_t) ch;   
#endif
	return ch;
}

void MyUSART_Init(void)
{
    __MYUSART_ENABLE_IT(UART_IT_RXNE);
}

void MyUSART_WriteChar(uint8_t ch)
{
#ifdef STM32H750xx
    while((MYUSART_CURR_USART->ISR&0X40)==0);
	MYUSART_CURR_USART->TDR=ch;
#endif
#ifdef STM32F407xx
    while((MYUSART_CURR_USART->SR&0X40)==0);
	MYUSART_CURR_USART->DR=ch;
#endif
}

uint8_t MyUSART_Write(uint8_t* str, uint16_t len)
{
    if(len>MYUSART_MAX_LEN)
        return 0;
    for(uint32_t i=0;i<len;i++)
    {
        MyUSART_WriteChar(*(str+i));
    }
    return len;
}
uint8_t MyUSART_WriteStr(uint8_t* str) // the \0 will BE sent;
{
    return MyUSART_WriteUntil(str,'\0');
}

uint8_t MyUSART_WriteLine(uint8_t* str) // the \0 of the string will not be sent, and the transimitted data ends with \r\n(no \0 at the end)
{
    uint32_t i;
    for(i=0;i<MYUSART_MAX_LEN;i++)
    {
        if(*(str+i)=='\0')
            break;
        MyUSART_WriteChar(*(str+i));
    }
    MyUSART_WriteChar('\r');
    MyUSART_WriteChar('\n');
    return i+2<MYUSART_MAX_LEN;
}

uint8_t MyUSART_WriteUntil(uint8_t* str,uint8_t endChar) // similar to the MyUSART_WriteStr() with the customized EndChar
{
    uint32_t i;
    for(i=0;i<MYUSART_MAX_LEN;i++)
    {
        MyUSART_WriteChar(*(str+i));
        if(*(str+i)==endChar)
            break;
    }
    return i<MYUSART_MAX_LEN;
}

uint8_t MyUSART_ReadChar(void)
{
    if(MyUSART_bufferPos!=0)
        MyUSART_bufferPos--;
    return MyUSART_buffer[MyUSART_bufferPos];
}

uint8_t MyUSART_PeekChar(void)
{
    if(MyUSART_bufferPos==0)
        return MyUSART_buffer[MYUSART_MAX_LEN-1];
    return MyUSART_buffer[MyUSART_bufferPos-1];
}

uint32_t MyUSART_Read(uint8_t* str, uint16_t maxLen)
{
    maxLen=maxLen<MyUSART_bufferPos?maxLen:MyUSART_bufferPos;
    uint32_t i,j;
    for(i=0;i<maxLen;i++)
    {
        *(str+i)=MyUSART_buffer[i];
    }
    __MyUSART_Shift(maxLen);
    return maxLen;
}

uint32_t MyUSART_ReadStr(uint8_t* str)
{
    return MyUSART_ReadUntil(str,'\0');
}

uint32_t MyUSART_ReadLine(uint8_t* str)
{
    uint32_t i,j;
    for(i=0;i<MyUSART_bufferPos;i++)
    {
        *(str+i)=MyUSART_buffer[i];
        if(i>0 && *(str+i-1)=='\r'&&*(str+i)=='\n')
        {
            i++;
            break;
        }
    }
    __MyUSART_Shift(i);
    return i;
}

uint32_t MyUSART_ReadUntil(uint8_t* str,uint16_t endChar)
{
    uint32_t i,j;
    for(i=0;i<MyUSART_bufferPos;i++)
    {
        *(str+i)=MyUSART_buffer[i];
        if(*(str+i)==endChar)
        {
            i++;
            break;
        }
    }
    __MyUSART_Shift(i);
    return i;
}

uint32_t MyUSART_ReadAll(uint8_t* str)
{
    uint32_t i;
    for(i=0;i<MyUSART_bufferPos;i++)
    {
        *(str+i)=MyUSART_buffer[i];
    }
    __MyUSART_Shift(i);
    return i;
}


void __MyUSART_Shift(uint32_t len)
{
    for(uint32_t i=0;i<MYUSART_MAX_LEN-len;i++)
    {
        MyUSART_buffer[i]=MyUSART_buffer[i+len];
    }
    MyUSART_bufferPos-=len;
}

void __MyUSART_DumpAll(void)
{
    if(MYUSART_MAX_LEN>100)
        return;
    MyUSART_WriteChar(MyUSART_bufferPos);
    for(uint32_t i=0;i<MYUSART_MAX_LEN;i++)
    {
        MyUSART_WriteChar(MyUSART_buffer[i]);
    }
}

void MyUSART_ClearBuffer()
{
    MyUSART_bufferPos=0;
}

void MyUSART_IRQHandler(USART_TypeDef* source)
{

#ifdef STM32H750xx
    if((MYUSART_CURR_USART->ISR&UART_FLAG_RXNE)==0)
        return;
	MyUSART_buffer[MyUSART_bufferPos++]=MYUSART_CURR_USART->RDR;  
#endif
#ifdef STM32F407xx
    if((MYUSART_CURR_USART->SR&UART_FLAG_RXNE)==0)
        return;
    MyUSART_buffer[MyUSART_bufferPos++]=MYUSART_CURR_USART->DR;  
#endif
    if(MyUSART_bufferPos==MYUSART_MAX_LEN)
        MyUSART_bufferPos=0;
}
