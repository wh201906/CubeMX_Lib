#include "SI446x.h"
#include "DELAY/delay.h"
#include "UTIL/mygpio.h"

const static uint8_t config_table[] = RADIO_CONFIGURATION_DATA_ARRAY;
uint16_t Si4463_delayTicks;
void (*Si4463_RxITHandler)(uint8_t len, uint8_t *data);

#define Si4463_Delay() Delay_ticks(Si4463_delayTicks)

/**
  * @brief :SI446x等待CTS状态
  * @param :无
  * @note  :无
  * @retval:无
  */
uint8_t SI446x_Wait_Cts(void)
{
  uint8_t l_Cts;
  uint16_t l_ReadCtsTimes = 0;

  // expected T for single read:
  // 2(bytes)*8(edges)*2(pos/negpulse)*50ns = 1.6us
  // max state change time: 15ms
  // wait for 9375 times
  do
  {
    SI4463_NSS(0); //SPI片选

    //读CTS状态
    SI446x_ReadWriteByte_Raw(READ_CMD_BUFF);
    l_Cts = SI446x_ReadWriteByte_Raw(0xFF);

    SI4463_NSS(1); //取消SPI片选

    if (9375 == l_ReadCtsTimes++)
      return 0;            // timeout
  } while (l_Cts != 0xFF); //直到读CTS的返回值等于0xFF
  return 1;                // CTS Received
}

/**
  * @brief :SI446x写命令
  * @param :
  *			@pCmd:命令首地址
  *			@CmdNumber：命令个数
  * @note  :无
  * @retval:无
  */
uint8_t SI446x_Write_Cmds(uint8_t *pCmd, uint8_t CmdNumber)
{
  if (!SI446x_Wait_Cts())
    return 0;

  SI4463_NSS(0); //SPI片选

  while (CmdNumber--)
  {
    SI446x_ReadWriteByte_Raw(*pCmd); //发送命令
    pCmd++;
  }

  SI4463_NSS(1); //取消SPI片选
  return 1;
}

/**
  * @brief :SI446x POWER_UP
  * @param :
  *			@Xo_Freq:晶振频率
  * @note  :SI446x复位之后需要调用
  * @retval:无
  */
void SI446x_Power_Up(uint32_t Xo_Freq)
{
  uint8_t l_Cmd[7] = {0};

  l_Cmd[0] = POWER_UP; //Power_Up命令
  l_Cmd[1] = 0x01;
  l_Cmd[2] = 0x00;
  l_Cmd[3] = Xo_Freq >> 24;
  l_Cmd[4] = Xo_Freq >> 16;
  l_Cmd[5] = Xo_Freq >> 8;
  l_Cmd[6] = Xo_Freq;
  SI446x_Write_Cmds(l_Cmd, 7); //写命令
}

/**
  * @brief :SI446x读CTS和命令应答
  * @param :
  *			@pRead:返回数据首地址
  *			@Length:长度
  * @note  :无
  * @retval:无
  */
uint8_t SI446x_Read_Response(uint8_t *pRead, uint8_t Length)
{
  if (!SI446x_Wait_Cts())
    return 0;
  SI4463_NSS(0); //SPI片选

  SI446x_ReadWriteByte_Raw(READ_CMD_BUFF); //发送读命令
  while (Length--)
  {
    *pRead = SI446x_ReadWriteByte_Raw(0xFF); //交换数据
    pRead++;
  }

  SI4463_NSS(1); //SPI取消片选
  return 1;
}

/**
  * @brief :SI446x空操作
  * @param :无
  * @note  :无
  * @retval:无
  */
uint8_t SI446x_Nop(void)
{
  uint8_t l_Cts;

  SI4463_NSS(0); //SPI片选

  l_Cts = SI446x_ReadWriteByte_Raw(NOP); //空操作命令

  SI4463_NSS(1); //SPI取消片选

  return l_Cts;
}

/**
  * @brief :SI446x读设备基本信息
  * @param :
  *			@pRead:返回数据首地址
  * @note  :无
  * @retval:无
  */
void SI446x_Get_Part_Informatoin(uint8_t *pRead)
{
  uint8_t l_Cmd = PART_INFO;

  SI446x_Write_Cmds(&l_Cmd, 1);   //命令
  SI446x_Read_Response(pRead, 8); //读设备基本信息
}

/**
  * @brief :SI446x读设备功能版本信息
  * @param :
  *			@pRead:返回数据首地址
  * @note  :无
  * @retval:无
  */
void SI446x_Get_Fun_Informatoin(uint8_t *pRead)
{
  uint8_t l_Cmd = FUNC_INFO;

  SI446x_Write_Cmds(&l_Cmd, 1);   //命令
  SI446x_Read_Response(pRead, 7); //读设备功能版本信息
}

/**
  * @brief :SI446x读中断状态
  * @param :
  *			@pRead:返回数据首地址
  * @note  :无
  * @retval:无
  */
void SI446x_Interrupt_Status(uint8_t *pRead)
{
  uint8_t l_Cmd[4] = {0};

  l_Cmd[0] = GET_INT_STATUS;
  l_Cmd[1] = 0;
  l_Cmd[2] = 0;
  l_Cmd[3] = 0;

  SI446x_Write_Cmds(l_Cmd, 4);    //发送中断读取命令
  SI446x_Read_Response(pRead, 9); //读取状态
}

/**
  * @brief :SI446x读取属性值
  * @param :
  *			@Group_Num:属性组(参考SI446X_PROPERTY)
  *			@Num_Props:读取的属性个数
  *			@pRead:返回数据首地址
  * @note  :无
  * @retval:无
  */
void SI446x_Get_Property(SI446X_PROPERTY Group_Num, uint8_t Num_Props, uint8_t *pRead)
{
  uint8_t l_Cmd[4] = {0};

  l_Cmd[0] = GET_PROPERTY;
  l_Cmd[1] = Group_Num >> 8;
  l_Cmd[2] = Num_Props;
  l_Cmd[3] = Group_Num;

  SI446x_Write_Cmds(l_Cmd, 4);                //发送读取属性命令
  SI446x_Read_Response(pRead, Num_Props + 1); //读属性
}

/**
  * @brief :SI446x设置属性
  * @param :
  *			@Group_Num:属性组(参考SI446X_PROPERTY)
  *			@Num_Props:设置的属性个数
  *			@pWrite:写地址设备
  * @note  :无
  * @retval:无
  */
void SI446x_Set_Property(SI446X_PROPERTY Group_Num, uint8_t Num_Props, uint8_t *pWrite)
{
  uint8_t l_Cmd[20] = {0}, i = 0;

  if (Num_Props >= 16)
  {
    return; //数量不大于16
  }

  l_Cmd[i++] = SET_PROPERTY; //设置属性命令
  l_Cmd[i++] = Group_Num >> 8;
  l_Cmd[i++] = Num_Props;
  l_Cmd[i++] = Group_Num;

  while (Num_Props--)
  {
    l_Cmd[i++] = *pWrite;
    pWrite++;
  }
  SI446x_Write_Cmds(l_Cmd, i); //发送命令及数据
}

/**
  * @brief :SI446x设置属性组1属性
  * @param :
  *			@Group_Num:属性组
  *			@Start_Prop:开始设置的属性号(参考SI446X_PROPERTY)
  * @note  :无
  * @retval:无
  */
void SI446x_Set_Property_1(SI446X_PROPERTY Group_Num, uint8_t Start_Prop)
{
  uint8_t l_Cmd[5] = {0};

  l_Cmd[0] = SET_PROPERTY; //命令
  l_Cmd[1] = Group_Num >> 8;
  l_Cmd[2] = 1;
  l_Cmd[3] = Group_Num;
  l_Cmd[4] = Start_Prop;

  SI446x_Write_Cmds(l_Cmd, 5); //发送命令设置属性
}

/**
  * @brief :SI446x读取属性组1属性
  * @param :
  *			@Group_Num:开始的属性号(参考SI446X_PROPERTY)
  * @note  :无
  * @retval:无
  */
uint8_t SI446x_Get_Property_1(SI446X_PROPERTY Group_Num)
{
  uint8_t l_Cmd[4] = {0};

  l_Cmd[0] = GET_PROPERTY;
  l_Cmd[1] = Group_Num >> 8;
  l_Cmd[2] = 1;
  l_Cmd[3] = Group_Num;
  SI446x_Write_Cmds(l_Cmd, 4); //发送命令

  SI446x_Read_Response(l_Cmd, 2); //读取属性

  return l_Cmd[1];
}

/**
  * @brief :SI446x复位
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Reset(void)
{
  SI4463_SDN(1); //关设备
  Delay_us(11);  //延时 等待设备完全断电
  SI4463_SDN(0); //开设备
  SI4463_NSS(1); //取消SPI片选
  Delay_us(1);
}

/**
  * @brief :SI446x配置GPIO
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Config_GPIO(uint8_t GPIO_0, uint8_t GPIO_1, uint8_t GPIO_2, uint8_t GPIO_3, uint8_t Irq, uint8_t Sdo, uint8_t Gen_Config)
{
  uint8_t l_Cmd[10] = {0};

  l_Cmd[0] = GPIO_PIN_CFG;
  l_Cmd[1] = GPIO_0;
  l_Cmd[2] = GPIO_1;
  l_Cmd[3] = GPIO_2;
  l_Cmd[4] = GPIO_3;
  l_Cmd[5] = Irq;
  l_Cmd[6] = Sdo;
  l_Cmd[7] = Gen_Config;

  SI446x_Write_Cmds(l_Cmd, 8);    //写配置
  SI446x_Read_Response(l_Cmd, 8); //读配置
}

/**
  * @brief :SI446x模块配置
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Config_Init(void)
{
  uint8_t i;
  uint16_t j = 0;

  while ((i = config_table[j]) != 0)
  {
    j += 1;
    SI446x_Write_Cmds((uint8_t *)config_table + j, i);
    j += i;
  }
#if PACKET_LENGTH > 0 //固定数据长度

  SI446x_Set_Property_1(PKT_FIELD_1_LENGTH_7_0, PACKET_LENGTH);

#else //动态数据长度

  SI446x_Set_Property_1(PKT_CONFIG1, 0x00);
  SI446x_Set_Property_1(PKT_CRC_CONFIG, 0x00);
  SI446x_Set_Property_1(PKT_LEN_FIELD_SOURCE, 0x01);
  SI446x_Set_Property_1(PKT_LEN, 0x2A);
  SI446x_Set_Property_1(PKT_LEN_ADJUST, 0x00);
  SI446x_Set_Property_1(PKT_FIELD_1_LENGTH_12_8, 0x00);
  SI446x_Set_Property_1(PKT_FIELD_1_LENGTH_7_0, 0x01);
  SI446x_Set_Property_1(PKT_FIELD_1_CONFIG, 0x00);
  SI446x_Set_Property_1(PKT_FIELD_1_CRC_CONFIG, 0x00);
  SI446x_Set_Property_1(PKT_FIELD_2_LENGTH_12_8, 0x00);
  SI446x_Set_Property_1(PKT_FIELD_2_LENGTH_7_0, 0x20);
  SI446x_Set_Property_1(PKT_FIELD_2_CONFIG, 0x00);
  SI446x_Set_Property_1(PKT_FIELD_2_CRC_CONFIG, 0x00);

#endif

  //4463 的GDO2 GDO3控制射频开关 33 32
  // 发射：GDO2 = 0, GDO3 = 1
  // 接收：GDO2 = 1, GDO3 = 0
  SI446x_Config_GPIO(0, 0, 33 | 0x40, 32 | 0x40, 0, 0, 0);
}

/**
  * @brief :SI446x写TX FIFO
  * @param :
  *			@pWriteData：写数据首地址
  *			@Length：数据长度
  * @note  :无
  * @retval:无
  */
void SI446x_Write_TxFifo(uint8_t *pWriteData, uint8_t Length)
{
  SI4463_NSS(0);
  SI446x_ReadWriteByte_Raw(WRITE_TX_FIFO); //写命令
  while (Length--)
  {
    SI446x_ReadWriteByte_Raw(*pWriteData++); //写数据
  }
  SI4463_NSS(1);
}

/**
  * @brief :SI446x 复位RX FIFO
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Reset_RxFifo(void)
{
  uint8_t l_Cmd[2] = {0};

  l_Cmd[0] = FIFO_INFO;
  l_Cmd[1] = 0x02;
  SI446x_Write_Cmds(l_Cmd, 2);
}

/**
  * @brief :SI446x 复位TX FIFO
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Reset_TxFifo(void)
{
  uint8_t l_Cmd[2] = {0};

  l_Cmd[0] = FIFO_INFO;
  l_Cmd[1] = 0x02;
  SI446x_Write_Cmds(l_Cmd, 2);
}

/**
  * @brief :SI446x发送数据包
  * @param :
  *			@pTxData：发送数据首地址
  *			@Length：数据长度
  *			@Channel：通道
  *			@Condition：发送状况选择
  * @note  :无
  * @retval:无
  */
void SI446x_Send_Packet(uint8_t *pTxData, uint8_t Length, uint8_t Channel, uint8_t Condition)
{
  uint8_t l_Cmd[5] = {0};
  uint8_t tx_len = Length;

  SI446x_Reset_TxFifo(); //清空TX FIFO

  SI4463_NSS(0);

  SI446x_ReadWriteByte_Raw(WRITE_TX_FIFO); //写TX FIFO命令

#if PACKET_LENGTH == 0 //动态数据长度

  tx_len++;
  SI446x_ReadWriteByte_Raw(Length);

#endif

  while (Length--)
  {
    SI446x_ReadWriteByte_Raw(*pTxData++); //写数据到TX FIFO
  }

  SI4463_NSS(1);

  l_Cmd[0] = START_TX;
  l_Cmd[1] = Channel;
  l_Cmd[2] = Condition;
  l_Cmd[3] = 0;
  l_Cmd[4] = tx_len;

  SI446x_Write_Cmds(l_Cmd, 5); //发送数据包
}

/**
  * @brief :SI446x启动发送
  * @param :
  *			@Length：数据长度
  *			@Channel：通道
  *			@Condition：发送状况选择
  * @note  :无
  * @retval:无
  */
void SI446x_Start_Tx(uint8_t Channel, uint8_t Condition, uint16_t Length)
{
  uint8_t l_Cmd[5] = {0};

  l_Cmd[0] = START_TX;
  l_Cmd[1] = Channel;
  l_Cmd[2] = Condition;
  l_Cmd[3] = Length >> 8;
  l_Cmd[4] = Length;

  SI446x_Write_Cmds(l_Cmd, 5);
}

/**
  * @brief :SI446x读RX FIFO数据
  * @param :
  *			@pRxData：数据首地址
  * @note  :无
  * @retval:数据个数
  */
uint8_t SI446x_Read_Packet(uint8_t *pRxData)
{
  uint8_t length = 0, i = 0;

  if (!SI446x_Wait_Cts())
    return 0;
  SI4463_NSS(0);

  SI446x_ReadWriteByte_Raw(READ_RX_FIFO); //读FIFO命令

#if PACKET_LENGTH == 0

  length = SI446x_ReadWriteByte_Raw(0xFF); //读数据长度

#else

  length = PACKET_LENGTH;

#endif
  i = length;

  while (length--)
  {
    *pRxData++ = SI446x_ReadWriteByte_Raw(0xFF); //读数据
  }

  SI4463_NSS(1); //返回数据个数

  return i;
}

/**
  * @brief :SI446x启动接收
  * @param :
  *			@Channel：通道
  *			@Condition：开始接收状态
  *			@Length：接收长度
  *			@Next_State1：下一个状态1
  *			@Next_State2：下一个状态2
  *			@Next_State3：下一个状态3
  * @note  :无
  * @retval:无
  */
void SI446x_Start_Rx(uint8_t Channel, uint8_t Condition, uint16_t Length, uint8_t Next_State1, uint8_t Next_State2, uint8_t Next_State3)
{
  uint8_t l_Cmd[8] = {0};

  SI446x_Reset_RxFifo();
  SI446x_Reset_TxFifo();

  l_Cmd[0] = START_RX;
  l_Cmd[1] = Channel;
  l_Cmd[2] = Condition;
  l_Cmd[3] = Length >> 8;
  l_Cmd[4] = Length;
  l_Cmd[5] = Next_State1;
  l_Cmd[6] = Next_State2;
  l_Cmd[7] = Next_State3;

  SI446x_Write_Cmds(l_Cmd, 8);
}

/**
  * @brief :SI446x读取当前数据包信息
  * @param :
  *			@pReadData：数据存放地址
  *			@FieldNumMask：掩码域
  *			@Length：长度
  *			@DiffLen：不同长度
  * @note  :无
  * @retval:无
  */
void SI446x_Get_Packet_Information(uint8_t *pReadData, uint8_t FieldNumMask, uint16_t Length, uint16_t DiffLen)
{
  uint8_t l_Cmd[6] = {0};

  l_Cmd[0] = PACKET_INFO;
  l_Cmd[1] = FieldNumMask;
  l_Cmd[2] = Length >> 8;
  l_Cmd[3] = Length;
  l_Cmd[4] = DiffLen >> 8;
  l_Cmd[5] = DiffLen;

  SI446x_Write_Cmds(l_Cmd, 6);
  SI446x_Read_Response(pReadData, 3);
}

/**
  * @brief :SI446x读取FIFO状态
  * @param :
  *			@pReadData：数据存放地址
  * @note  :无
  * @retval:无
  */
void SI446x_Get_Fifo_Information(uint8_t *pReadData)
{
  uint8_t l_Cmd[2] = {0};

  l_Cmd[0] = FIFO_INFO;
  l_Cmd[1] = 0x03;

  SI446x_Write_Cmds(l_Cmd, 2);
  SI446x_Read_Response(pReadData, 3);
}

/**
  * @brief :SI446x状态切换
  * @param :
  *			@NextStatus：下一个状态
  * @note  :无
  * @retval:无
  */
void SI446x_Change_Status(uint8_t NextStatus)
{
  uint8_t l_Cmd[2] = {0};

  l_Cmd[0] = CHANGE_STATE;
  l_Cmd[1] = NextStatus;

  SI446x_Write_Cmds(l_Cmd, 2);
}

/**
  * @brief :SI446x获取设备当前状态
  * @param :
  * @note  :无
  * @retval:设备当前状态
  */
uint8_t SI446x_Get_Device_Status(void)
{
  uint8_t l_Cmd[3] = {0};

  l_Cmd[0] = REQUEST_DEVICE_STATE;

  SI446x_Write_Cmds(l_Cmd, 1);
  SI446x_Read_Response(l_Cmd, 3);

  return l_Cmd[1] & 0x0F;
}

/**
  * @brief :SI446x功率设置
  * @param :
  *			@PowerLevel：数据存放地址
  * @note  :无
  * @retval:设备当前状态
  */
void SI446x_Set_Power(uint8_t PowerLevel)
{
  SI446x_Set_Property_1(PA_PWR_LVL, PowerLevel);
}

/**
  * @brief :SI446x引脚初始化
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  //打开引脚端口时钟
  SI4463_CLK_CLKEN();
  SI4463_MISO_CLKEN();
  SI4463_MOSI_CLKEN();
  SI4463_NSS_CLKEN();
  SI4463_SDN_CLKEN();
  SI4463_IRQ_CLKEN();
  SI4463_GPIO0_CLKEN();
  SI4463_GPIO1_CLKEN();
  SI4463_GPIO2_CLKEN();
  SI4463_GPIO3_CLKEN();

  //SDN 引脚配置为推挽输出
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  MyGPIO_Init(SI4463_CLK_PORT, SI4463_CLK_PIN, 0);
  MyGPIO_Init(SI4463_MOSI_PORT, SI4463_MOSI_PIN, 0);
  MyGPIO_Init(SI4463_NSS_PORT, SI4463_NSS_PIN, 1);
  MyGPIO_Init(SI4463_SDN_PORT, SI4463_SDN_PIN, 0);
  MyGPIO_Init(SI4463_GPIO2_PORT, SI4463_GPIO2_PIN, 1); // Tx

  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  MyGPIO_Init(SI4463_MISO_PORT, SI4463_MISO_PIN, 0);
  MyGPIO_Init(SI4463_IRQ_PORT, SI4463_IRQ_PIN, 1);
  // MyGPIO_Init(SI4463_GPIO0_PORT, SI4463_GPIO0_PIN, 1); // RxClk
  MyGPIO_Init(SI4463_GPIO1_PORT, SI4463_GPIO1_PIN, 1); // Rx
  // MyGPIO_Init(SI4463_GPIO2_PORT, SI4463_GPIO2_PIN, 1);
  // MyGPIO_Init(SI4463_GPIO3_PORT, SI4463_GPIO3_PIN, 1); TxClk
}

/**
  * @brief :SI446x初始化
  * @param :无
  * @note  :无
  * @retval:无
  */
void SI446x_Init(void)
{
  Si4463_delayTicks = Delay_GetSYSFreq() * 0.000000050 + 1.0; // 50ns, to meet select hold time
  SI446x_GPIO_Init();
  Delay_us(1);
  SI446x_Reset();
  SI446x_Power_Up(30000000);
  SI446x_Config_Init();
  SI446x_Set_Power(0x7F);
  SI446x_Change_Status(6); // Rx mode
  while (SI446x_Get_Device_Status() != 6)
    ;
  SI446x_Start_Rx(0, 0, PACKET_LENGTH, 0, 0, 3);
}

/**
  * @brief :SPI收发一个字节
  * @param :
  *			@TxByte: 发送的数据字节
  * @note  :非堵塞式，一旦等待超时，函数会自动退出
  * @retval:接收到的字节
  */
uint8_t SI446x_ReadWriteByte_Raw(uint8_t TxByte)
{
  uint8_t i = 0, RxByte = 0;

  SI4463_CLK(0);

  for (i = 0; i < 8; i++) //一个字节8byte需要循环8次
  {
    SI4463_MOSI(!!(TxByte & 0x80));
    TxByte <<= 1;

    SI4463_CLK(1);
    Si4463_Delay();

    RxByte <<= 1;
    RxByte |= SI4463_MISO();

    SI4463_CLK(0);
    Si4463_Delay();
  }

  return RxByte; //返回接收到的字节
}

/**
  * @brief :SPI收发字符串
  * @param :
  *			@ReadBuffer: 接收数据缓冲区地址
  *			@WriteBuffer:发送字节缓冲区地址
  *			@Length:字节长度
  * @note  :非堵塞式，一旦等待超时，函数会自动退出
  * @retval:无
  */
void SI446x_ReadWrite_Raw(uint8_t *ReadBuffer, uint8_t *WriteBuffer, uint16_t Length)
{
  SI4463_NSS(0);

  while (Length--)
  {
    *ReadBuffer = SI446x_ReadWriteByte_Raw(*WriteBuffer); //收发数据
    ReadBuffer++;
    WriteBuffer++; //读写地址加1
  }

  SI4463_NSS(1);
}

void SI446x_Read_Packet_IT(void)
{
  uint8_t RxData[65]; // maybe 128 in mixed FIFO?
  uint8_t status;
  uint8_t length = 0, i = 0;

  SI4463_NSS(0);
  SI446x_ReadWriteByte_Raw(READ_RX_FIFO); //读FIFO命令

#if PACKET_LENGTH == 0
  length = SI446x_ReadWriteByte_Raw(0xFF); //读数据长度
  if (length > 64)
  {
    length = 64;
  }
#else
  length = PACKET_LENGTH;
#endif
  for (i = 0; i < length; i++)
    RxData[i] = SI446x_ReadWriteByte_Raw(0xFF); //读数据
  SI446x_Clear_RxIT();
  SI4463_NSS(1);

  if (Si4463_RxITHandler != NULL)
    Si4463_RxITHandler(length, RxData);
  
  SI446x_Start_Rx(0, 0, PACKET_LENGTH, 0, 0, 3);
}

void SI446x_Clear_RxIT(void)
{
  uint8_t l_Cmd[3] = {0};

  l_Cmd[0] = GET_PH_STATUS;
  l_Cmd[1] = 0x10; // PACKET_RX

  SI446x_Write_Cmds(l_Cmd, 2);
  SI446x_Read_Response(l_Cmd, 3);
}

void SI446x_RegisterRxITHandler(void (*handler)(uint8_t len, uint8_t *data))
{
  Si4463_RxITHandler = handler;
}

// Used for IRQ based receive
// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
// {
//   if (GPIO_Pin != SI4463_IRQ_PIN || SI4463_IRQ())
//     return;
//   SI446x_Read_Packet_IT();
// }