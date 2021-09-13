#ifndef _NODE_H_
#define _NODE_H_

#include "main.h"
#include "UART/myuart.h"
#ifndef NODE_ID
#define NODE_ID '0' // '0':server, '1':client1, '2':client2
#endif

extern uint64_t updateTimes;
extern MyUARTHandle uart1, uart2;

#define NODE_UART &uart2

#define Node_GetTicks() (updateTimes << 16 | LL_TIM_GetCounter(TIM10))
#define Node_Write(__STR__, __LEN__) MyUART_Write(NODE_UART, (__STR__), (__LEN__))
#define Node_ReadUntil(__STR__, __ENDCHAR__) MyUART_ReadUntil(NODE_UART, (__STR__), (__ENDCHAR__))

void Node_EventLoop(void);
void Node_PacketHead(uint8_t target);
uint64_t Node_GetLatency(uint8_t target);
void Node_AckLatency(void);


#endif