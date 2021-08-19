#ifndef __DRV_SPI_H__
#define __DRV_SPI_H__

#include "main.h"

//SPI引脚定义
#define SPI_CLK_GPIO_PORT GPIOB
#define SPI_CLK_GPIO_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI_CLK_GPIO_PIN GPIO_PIN_3

#define SPI_MISO_GPIO_PORT GPIOB
#define SPI_MISO_GPIO_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI_MISO_GPIO_PIN GPIO_PIN_4

#define SPI_MOSI_GPIO_PORT GPIOB
#define SPI_MOSI_GPIO_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI_MOSI_GPIO_PIN GPIO_PIN_5

#define SPI_NSS_GPIO_PORT GPIOB
#define SPI_NSS_GPIO_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI_NSS_GPIO_PIN GPIO_PIN_7

#define NRF24L01_NSS(__PINSTATE__) (SPI_NSS_GPIO_PORT->BSRR = (uint32_t)(SPI_NSS_GPIO_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define NRF24L01_CLK(__PINSTATE__) (SPI_CLK_GPIO_PORT->BSRR = (uint32_t)(SPI_CLK_GPIO_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define NRF24L01_MOSI(__PINSTATE__) (SPI_MOSI_GPIO_PORT->BSRR = (uint32_t)(SPI_MOSI_GPIO_PIN) << ((__PINSTATE__) ? (0u) : (16u)))
#define NRF24L01_MISO() (!!(SPI_MISO_GPIO_PORT->IDR & SPI_MISO_GPIO_PIN))

void drv_spi_init(void);
uint8_t drv_spi_read_write_byte(uint8_t TxByte);
void drv_spi_read_write_string(uint8_t *ReadBuffer, uint8_t *WriteBuffer, uint16_t Length);

#endif
