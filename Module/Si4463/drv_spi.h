/**
  ******************************************************************************
  * @author  泽耀科技 ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   SPI配置H文件
  ******************************************************************************
  * @attention
  *
  * 官网	:	http://www.ashining.com
  * 淘宝	:	https://shop105912646.taobao.com
  * 阿里巴巴:	https://cdzeyao.1688.com
  ******************************************************************************
  */


#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

#include "main.h"



//SPI引脚定义
#define SPI_CLK_GPIO_PORT			GPIOD
#define SPI_CLK_GPIO_CLKEN()	__HAL_RCC_GPIOD_CLK_ENABLE()
#define SPI_CLK_GPIO_PIN			GPIO_PIN_11

#define SPI_MISO_GPIO_PORT		GPIOD
#define SPI_MISO_GPIO_CLKEN()	__HAL_RCC_GPIOD_CLK_ENABLE()
#define SPI_MISO_GPIO_PIN			GPIO_PIN_12

#define SPI_MOSI_GPIO_PORT		GPIOD
#define SPI_MOSI_GPIO_CLKEN()	__HAL_RCC_GPIOD_CLK_ENABLE()
#define SPI_MOSI_GPIO_PIN			GPIO_PIN_13

#define SPI_NSS_GPIO_PORT			GPIOD
#define SPI_NSS_GPIO_CLKEN()	__HAL_RCC_GPIOD_CLK_ENABLE()
#define SPI_NSS_GPIO_PIN			GPIO_PIN_14


#define spi_set_nss_high( )			SPI_NSS_GPIO_PORT->ODR |= SPI_NSS_GPIO_PIN								//片选置高
#define spi_set_nss_low( )			SPI_NSS_GPIO_PORT->ODR &= (uint32_t)( ~((uint32_t)SPI_NSS_GPIO_PIN ))	//片选置低
		

#define spi_set_clk_high( )			SPI_CLK_GPIO_PORT->ODR |= SPI_CLK_GPIO_PIN								//时钟置高
#define spi_set_clk_low( )			SPI_CLK_GPIO_PORT->ODR &= (uint32_t)( ~((uint32_t)SPI_CLK_GPIO_PIN ))	//时钟置低

#define spi_set_mosi_hight( )		SPI_MOSI_GPIO_PORT->ODR |= SPI_MOSI_GPIO_PIN							//发送脚置高
#define spi_set_mosi_low( )			SPI_MOSI_GPIO_PORT->ODR &= (uint32_t)( ~((uint32_t)SPI_MOSI_GPIO_PIN ))	//发送脚置低

#define spi_get_miso( )				(( SPI_MISO_GPIO_PORT->IDR & (uint32_t)SPI_MISO_GPIO_PIN) != SPI_MISO_GPIO_PIN ) ? 0 : 1 // 若相应输入位为低则得到0，相应输入位为高则得到1


void drv_spi_init( void );
uint8_t drv_spi_read_write_byte( uint8_t TxByte );
void drv_spi_read_write_string( uint8_t* ReadBuffer, uint8_t* WriteBuffer, uint16_t Length );



#endif

