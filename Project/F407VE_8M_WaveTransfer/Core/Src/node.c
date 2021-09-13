#include "node.h"

uint64_t updateTimes = 0;
uint8_t nodeBuf[100]; // [0]:source [1]:target [2:]:payload
#define NODE_PACKET_SRC nodeBuf[0]
#define NODE_PACKET_DST nodeBuf[1]

void Node_PacketHead(uint8_t target)
{
  MyUART_WriteChar(NODE_UART, NODE_ID);
  MyUART_WriteChar(NODE_UART, target);
}

void Node_EventLoop(void)
{
  if(Node_ReadUntil(nodeBuf, '>') && nodeBuf[1] == NODE_ID)
  {
    Node_AckLatency();
  }
}

uint64_t Node_GetLatency(uint8_t target)
{
  uint32_t testCnt = 0;
  uint64_t start, start2;
  start = Node_GetTicks();
  __NOP();
  start2 = Node_GetTicks();
  MyUART_ClearBuffer(NODE_UART);
  Node_PacketHead(target);
  Node_Write("1>", 2);
  while(1)
  {
    if(!Node_ReadUntil(nodeBuf, '>'))
      continue;
    if(NODE_PACKET_SRC == target && NODE_PACKET_DST == NODE_ID && nodeBuf[2] == '1')
      break;
  }
  uint64_t test = Node_GetTicks(), test2;
  __NOP();
  test2 = Node_GetTicks();
  if(test - start > 19000 || test - start < 14500)
    printf("%llu, %llu, %llu %llu, %llu, %llu\n", start, test, test-start,start2, test2, test2-start2);
  return (test - start);
}

void Node_AckLatency(void)
{
  if(nodeBuf[2] == '1')
  {
    Node_PacketHead(NODE_PACKET_SRC);
    Node_Write("1>", 2);
  }
}