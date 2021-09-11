#include "node.h"

uint64_t updateTimes = 0;
uint8_t nodeBuf[100]; // [0]:source [1]:target [2:]:payload

void Node_PacketHead(uint8_t target)
{
  MyUART_WriteChar(NODE_UART, NODE_ID);
  MyUART_WriteChar(NODE_UART, target);
}

void Node_EventLoop(void)
{
  if(Node_ReadUntil(nodeBuf, '>') && nodeBuf[1] == NODE_ID)
  {
    Node_AckDelay();
  }
}

uint64_t Node_GetDelay(uint8_t target)
{
  uint64_t start;
  start = Node_getCurrTick();
  MyUART_ClearBuffer(NODE_UART);
  Node_PacketHead(target);
  Node_Write("1>", 2);
  while(1)
  {
    if(!Node_ReadUntil(nodeBuf, '>'))
      continue;
    if(nodeBuf[0] == target && nodeBuf[1] == NODE_ID && nodeBuf[2] == '1')
      break;
  }
  return (Node_getCurrTick() - start);
}

void Node_AckDelay(void)
{
  if(nodeBuf[2] == '1')
  {
    Node_PacketHead(nodeBuf[0]);
    Node_Write("1>", 2);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  updateTimes++;
}