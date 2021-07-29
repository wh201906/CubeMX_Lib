/**
  ******************************************************************************
  * @author  泽耀科技 ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   SPI配置C文件
  ******************************************************************************
  * @attention
  *
  * 官网	:	http://www.ashining.com
  * 淘宝	:	https://shop105912646.taobao.com
  * 阿里巴巴:	https://cdzeyao.1688.com
  ******************************************************************************
  */



#include "drv_spi.h"
#include "DELAY/delay.h"

/** 软件SPI */


/**
  * @brief :SPI初始化(软件)
  * @param :无
  * @note  :无
  * @retval:无
  */
void drv_spi_init( void )
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/** SPI引脚配置 */
  SPI_CLK_GPIO_CLKEN();
  SPI_MISO_GPIO_CLKEN();
  SPI_MOSI_GPIO_CLKEN();
  SPI_NSS_GPIO_CLKEN();
	
	//SCK MOSI NSS配置为推挽输出
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	HAL_GPIO_WritePin(SPI_CLK_GPIO_PORT, SPI_CLK_GPIO_PIN, 0);
	GPIO_InitStruct.Pin = SPI_CLK_GPIO_PIN;
	HAL_GPIO_Init(SPI_CLK_GPIO_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(SPI_MOSI_GPIO_PORT, SPI_MOSI_GPIO_PIN, 1);
	GPIO_InitStruct.Pin = SPI_MOSI_GPIO_PIN;
	HAL_GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(SPI_NSS_GPIO_PORT, SPI_NSS_GPIO_PIN, 1);
	GPIO_InitStruct.Pin = SPI_NSS_GPIO_PIN;
	HAL_GPIO_Init(SPI_NSS_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pin = SPI_MISO_GPIO_PIN;
	HAL_GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief :SPI收发一个字节
  * @param :
  *			@TxByte: 发送的数据字节
  * @note  :非堵塞式，一旦等待超时，函数会自动退出
  * @retval:接收到的字节
  */
uint8_t drv_spi_read_write_byte( uint8_t TxByte )
{
	uint8_t i = 0, Data = 0;
	
	spi_set_clk_low( );
	
	for( i = 0; i < 8; i++ )			//一个字节8byte需要循环8次
	{
		/** 发送 */
		if( 0x80 == ( TxByte & 0x80 ))
		{
			spi_set_mosi_hight( );		//如果即将要发送的位为 1 则置高IO引脚
		}
		else
		{
			spi_set_mosi_low( );		//如果即将要发送的位为 0 则置低IO引脚
		}
		TxByte <<= 1;					//数据左移一位，先发送的是最高位
		
		spi_set_clk_high( );			//时钟线置高
		__nop( );
		__nop( );
		
		/** 接收 */
		Data <<= 1;						//接收数据左移一位，先接收到的是最高位
		if( 1 == spi_get_miso( ))
		{
			Data |= 0x01;				//如果接收时IO引脚为高则认为接收到 1
		}
		
		spi_set_clk_low( );				//时钟线置低
		__nop( );
		__nop( );
	}
	
	return Data;		//返回接收到的字节
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
void drv_spi_read_write_string( uint8_t* ReadBuffer, uint8_t* WriteBuffer, uint16_t Length )
{
	spi_set_nss_low( );			//片选拉低
	
	while( Length-- )
	{
		*ReadBuffer = drv_spi_read_write_byte( *WriteBuffer );		//收发数据
		ReadBuffer++;
		WriteBuffer++;			//读写地址加1
	}
	
	spi_set_nss_high( );		//片选拉高
}


/** 软件SPI */


