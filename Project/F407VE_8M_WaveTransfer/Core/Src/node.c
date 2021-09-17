#include "node.h"
#include "PARAIO/paraio.h"
#include "math.h"

volatile uint64_t updateTimes = 0;
uint8_t nodeBuf[100]; // [0]:source [1]:target [2:]:payload
#define NODE_PACKET_SRC nodeBuf[0]
#define NODE_PACKET_DST nodeBuf[1]

#define DACLEN 200
#define PI 3.1415926535
uint8_t dacVal[DACLEN];

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
  uint64_t start1, start2, end1, end2;
  start1 = Node_GetTicks();
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
  end1 = Node_GetTicks();
  end2 = Node_GetTicks();
  if(start1 > start2)
    start2 += LL_TIM_GetAutoReload(TIM10) + 1;
  if(end1 > end2)
    end2 += LL_TIM_GetAutoReload(TIM10) + 1;
  
  return (end2 - start2);
}

void Node_AckLatency(void)
{
  if(nodeBuf[2] == '1')
  {
    Node_PacketHead(NODE_PACKET_SRC);
    Node_Write("1>", 2);
  }
}

void Node_Replay_SinDemo(uint16_t len)
{
  uint16_t i;
  for(i = 0; i < len; i++)
  {
    dacVal[i] = (double)sin(2 * PI * (double)i / DACLEN) * 127 + 128;
  }
}

void Node_Replay_Init(void)
{
  ParaIO_Init_Out(&htim8, 8, 1);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
  Node_Replay_SinDemo(DACLEN);
  ParaIO_Start_Out(dacVal, DACLEN);
}

void Node_Acquire_Init(void)
{
  ParaIO_Init_In(&htim8, 8, 0);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
}
