#ifndef __NRF24L01_H__
#define __NRF24L01_H__

#include "main.h"

/** 配置和选项定义 */
#define DYNAMIC_PACKET 1    //1:动态数据包, 0:固定
#define FIXED_PACKET_LEN 32 //包长度
#define REPEAT_CNT 15       //重复次数
#define INIT_ADDR 0x34, 0x43, 0x10, 0x10, 0x01

/** RF24L01硬件接口定义 */
#define NRF24L01_CLK_PORT GPIOB
#define NRF24L01_CLK_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define NRF24L01_CLK_PIN GPIO_PIN_3

#define NRF24L01_MISO_PORT GPIOB
#define NRF24L01_MISO_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define NRF24L01_MISO_PIN GPIO_PIN_4

#define NRF24L01_MOSI_PORT GPIOB
#define NRF24L01_MOSI_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define NRF24L01_MOSI_PIN GPIO_PIN_5

#define NRF24L01_CS_PORT GPIOB
#define NRF24L01_CS_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define NRF24L01_CS_PIN GPIO_PIN_7

#define NRF24L01_CE_PORT GPIOB
#define NRF24L01_CE_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define NRF24L01_CE_PIN GPIO_PIN_6

#define NRF24L01_IRQ_PORT GPIOB
#define NRF24L01_IRQ_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define NRF24L01_IRQ_PIN GPIO_PIN_8

#define NRF24L01_CLK(__PINSTATE__) (NRF24L01_CLK_PORT->BSRR = (uint32_t)(NRF24L01_CLK_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define NRF24L01_MOSI(__PINSTATE__) (NRF24L01_MOSI_PORT->BSRR = (uint32_t)(NRF24L01_MOSI_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define NRF24L01_MISO() (!!(NRF24L01_MISO_PORT->IDR & NRF24L01_MISO_PIN))
#define NRF24L01_CS(__PINSTATE__) (NRF24L01_CS_PORT->BSRR = (uint32_t)(NRF24L01_CS_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define NRF24L01_CE(__PINSTATE__) (NRF24L01_CE_PORT->BSRR = (uint32_t)(NRF24L01_CE_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define NRF24L01_IRQ() (!!(NRF24L01_IRQ_PORT->IDR & NRF24L01_IRQ_PIN))

typedef enum ModeType
{
  MODE_TX = 0,
  MODE_RX
} nRf24l01ModeType;

typedef enum SpeedType
{
  SPEED_250K = 0,
  SPEED_1M,
  SPEED_2M
} nRf24l01SpeedType;

typedef enum PowerType
{
  POWER_F18DBM = 0,
  POWER_F12DBM,
  POWER_F6DBM,
  POWER_0DBM
} nRf24l01PowerType;

/** NRF24L01定义 */
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//寄存器操作命令
#define NRF_READ_REG 0x00  //读配置寄存器，低5位为寄存器地址
#define NRF_WRITE_REG 0x20 //写配置寄存器，低5位为寄存器地址
#define RD_RX_PLOAD 0x61   //读RX有效数据，1~32字节
#define WR_TX_PLOAD 0xA0   //写TX有效数据，1~32字节
#define FLUSH_TX 0xE1      //清除TX FIFO寄存器，发射模式下使用
#define FLUSH_RX 0xE2      //清除RX FIFO寄存器，接收模式下使用
#define REUSE_TX_PL 0xE3   //重新使用上一包数据，CE为高，数据包被不断发送
#define R_RX_PL_WID 0x60
#define NOP 0xFF //空操作，可以用来读状态寄存器
#define W_ACK_PLOAD 0xA8
#define WR_TX_PLOAD_NACK 0xB0
//SPI(NRF24L01)寄存器地址
#define CONFIG 0x00     //配置寄存器地址，bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能; \
                        //bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define EN_AA 0x01      //使能自动应答功能 bit0~5 对应通道0~5
#define EN_RXADDR 0x02  //接收地址允许 bit0~5 对应通道0~5
#define SETUP_AW 0x03   //设置地址宽度(所有数据通道) bit0~1: 00,3字节 01,4字节, 02,5字节
#define SETUP_RETR 0x04 //建立自动重发;bit0~3:自动重发计数器;bit4~7:自动重发延时 250*x+86us
#define RF_CH 0x05      //RF通道,bit0~6工作通道频率
#define RF_SETUP 0x06   //RF寄存器，bit3:传输速率( 0:1M 1:2M);bit1~2:发射功率;bit0:噪声放大器增益
#define STATUS 0x07     //状态寄存器;bit0:TX FIFO满标志;bit1~3:接收数据通道号(最大:6);bit4:达到最多次重发次数 \
                        //bit5:数据发送完成中断;bit6:接收数据中断

#define OBSERVE_TX 0x08      //发送检测寄存器,bit7~4:数据包丢失计数器;bit3~0:重发计数器
#define CD 0x09              //载波检测寄存器,bit0:载波检测
#define RX_ADDR_P0 0x0A      //数据通道0接收地址，最大长度5个字节，低字节在前
#define RX_ADDR_P1 0x0B      //数据通道1接收地址，最大长度5个字节，低字节在前
#define RX_ADDR_P2 0x0C      //数据通道2接收地址，最低字节可设置，高字节，必须同RX_ADDR_P1[39:8]相等
#define RX_ADDR_P3 0x0D      //数据通道3接收地址，最低字节可设置，高字节，必须同RX_ADDR_P1[39:8]相等
#define RX_ADDR_P4 0x0E      //数据通道4接收地址，最低字节可设置，高字节，必须同RX_ADDR_P1[39:8]相等
#define RX_ADDR_P5 0x0F      //数据通道5接收地址，最低字节可设置，高字节，必须同RX_ADDR_P1[39:8]相等
#define TX_ADDR 0x10         //发送地址(低字节在前),ShockBurstTM模式下，RX_ADDR_P0与地址相等
#define RX_PW_P0 0x11        //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P1 0x12        //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P2 0x13        //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P3 0x14        //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P4 0x15        //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P5 0x16        //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define NRF_FIFO_STATUS 0x17 //FIFO状态寄存器;bit0:RX FIFO寄存器空标志;bit1:RX FIFO满标志;bit2~3保留 \
                             //bit4:TX FIFO 空标志;bit5:TX FIFO满标志;bit6:1,循环发送上一数据包.0,不循环
#define DYNPD 0x1C
#define FEATRUE 0x1D
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//位定义
#define EN_CRC 3
#define CRCO 2
#define PWR_UP 1
#define PRIM_RX 0

#define ENAA_P5 5
#define ENAA_P4 4
#define ENAA_P3 3
#define ENAA_P2 2
#define ENAA_P1 1
#define ENAA_P0 0

#define ERX_P5 5
#define ERX_P4 4
#define ERX_P3 3
#define ERX_P2 2
#define ERX_P1 1
#define ERX_P0 0

#define AW_RERSERVED 0x0
#define AW_3BYTES 0x1
#define AW_4BYTES 0x2
#define AW_5BYTES 0x3

#define ARD_250US (0x00 << 4)
#define ARD_500US (0x01 << 4)
#define ARD_750US (0x02 << 4)
#define ARD_1000US (0x03 << 4)
#define ARD_2000US (0x07 << 4)
#define ARD_4000US (0x0F << 4)
#define ARC_DISABLE 0x00
#define ARC_15 0x0F

#define CONT_WAVE 7
#define RF_DR_LOW 5
#define PLL_LOCK 4
#define RF_DR_HIGH 3
//bit2-bit1:
#define PWR_18DB (0x00 << 1)
#define PWR_12DB (0x01 << 1)
#define PWR_6DB (0x02 << 1)
#define PWR_0DB (0x03 << 1)

//for bit3-bit1,
#define TX_FULL_0 0

#define RPD 0

#define TX_REUSE 6
#define TX_FULL_1 5
#define TX_EMPTY 4
//bit3-bit2, reserved, only '00'
#define RX_FULL 1
#define RX_EMPTY 0

#define DPL_P5 5
#define DPL_P4 4
#define DPL_P3 3
#define DPL_P2 2
#define DPL_P1 1
#define DPL_P0 0

#define EN_DPL 2
#define EN_ACK_PAY 1
#define EN_DYN_ACK 0

// Used for enable/disable/clear/read interrupt
#define NRF24L01_RX_DR (1 << 6)
#define NRF24L01_TX_DS (1 << 5)
#define NRF24L01_MAX_RT (1 << 4)
#define NRF24L01_IRQ_MASK (NRF24L01_RX_DR | NRF24L01_TX_DS | NRF24L01_MAX_RT)

uint8_t NRF24L01_Read_Reg(uint8_t RegAddr);
void NRF24L01_Read_Buf(uint8_t RegAddr, uint8_t *pBuf, uint8_t len);
void NRF24L01_Write_Reg(uint8_t RegAddr, uint8_t Value);
void NRF24L01_Write_Buf(uint8_t RegAddr, uint8_t *pBuf, uint8_t len);
void NRF24L01_Flush_Tx_Fifo(void);
void NRF24L01_Flush_Rx_Fifo(void);
void NRF24L01_Reuse_Tx_Payload(void);
void NRF24L01_Nop(void);
uint8_t NRF24L01_Read_Status_Register(void);
uint8_t NRF24L01_Read_Top_Fifo_Width(void);
uint8_t NRF24L01_Read_Rx_Payload(uint8_t *pRxBuf);
void NRF24L01_Write_Tx_Payload_Ack(uint8_t *pTxBuf, uint8_t len);
void NRF24L01_Write_Tx_Payload_NoAck(uint8_t *pTxBuf, uint8_t len);
void NRF24L01_Write_Tx_Payload_InAck(uint8_t *pData, uint8_t len);
void NRF24L01_Set_TxAddr(uint8_t *pAddr, uint8_t len);
void NRF24L01_Set_RxAddr(uint8_t PipeNum, uint8_t *pAddr, uint8_t Len);
void NRF24L01_Set_Speed(nRf24l01SpeedType Speed);
void NRF24L01_Set_Power(nRf24l01PowerType Power);
void NRF24L01_Write_Hopping_Point(uint8_t FreqPoint);
void NRF24L01_Set_Mode(nRf24l01ModeType Mode);
uint8_t NRF24L01_Check(void);
uint8_t NRF24L01_TxPacket(uint8_t *txbuf, uint8_t Length);
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf);
void NRF24L01_GPIO_Init(void);
uint8_t NRF24L01_Init(void);

uint8_t NRF24L01_ReadWriteByte_Raw(uint8_t TxByte);

void NRF24L01_ReadWrite_Raw(uint8_t *ReadBuffer, uint8_t *WriteBuffer, uint32_t Length);
uint8_t NRF24L01_EnableIRQ(uint8_t irq);
uint8_t NRF24L01_DisableIRQ(uint8_t irq);
uint8_t NRF24L01_ClearIRQ(uint8_t irq);
uint8_t NRF24L01_ReadIRQ(void);
uint8_t NRF24L01_IsIRQ(uint8_t irq);

void NRF24L01_RxPacket_IT(void);                                                // put it in the EXTI IRQHandler
void NRF24L01_RegisterRxITHandler(void (*handler)(uint8_t len, uint8_t *data)); // the parameter is a function pointer

uint8_t NRF24L01_ReadStatus(void);

#endif
