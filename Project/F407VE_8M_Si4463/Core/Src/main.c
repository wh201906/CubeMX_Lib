/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Si4463/SI446x.h"
#include "KEY/key.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
MyUARTHandle uart1;
uint8_t uartBuf1[100];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void RxHandler(uint8_t len, uint8_t* data)
{
  MyUART_Write(&uart1, data, len);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t g_SI4463ItStatus[9] = {0};
  uint8_t g_SI4463RxBuffer[64] = {0};
  uint32_t i, fail;
  uint8_t status;
  uint8_t mode = 0, lastmode = 1;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  Delay_Init(168);
  MyUART_Init(&uart1, USART1, uartBuf1, 100);
  Key_Init();
  printf("Si4463 Test\r\n");
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
  SI446x_Init();
  SI446x_RegisterRxITHandler(&RxHandler);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  printf("mode: %s\r\n", mode ? "Tx" : "Rx");
  fail = 0;
  
  for(i = 0; i < 6; i++)
  {
    if(Key_ScanRaw() != 0xFF)
    {
      mode = !mode;
      printf("mode: %s\r\n", mode ? "Tx" : "Rx");
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, !mode);
    }
    Delay_ms(500);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(Key_ScanRaw() != 0xFF)
    {
      mode = !mode;
      printf("mode: %s\r\n", mode ? "Tx" : "Rx");
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, !mode);
    }
    if(lastmode != mode)
    {
      // on switch logic
      if(mode) // tx
      {
        HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
      }
      else
      {
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
        SI446x_Start_Rx(0, 0, PACKET_LENGTH, 0, 0, 0);
      }
    }
    lastmode = mode;
    if(mode)
    {
      SI446x_Send_Packet("0123456789ABCDEF9876543210abcdef0246813579fedcba1357902468FEDCBA",64,0,0);
      Delay_ms(1000);
    }
    else
    {
      SI446x_Interrupt_Status(g_SI4463ItStatus);
      printf("%x %x %x %x %x %x %x %x|%x\r\n", 
      g_SI4463ItStatus[1],
      g_SI4463ItStatus[2],
      g_SI4463ItStatus[3],
      g_SI4463ItStatus[4],
      g_SI4463ItStatus[5],
      g_SI4463ItStatus[6],
      g_SI4463ItStatus[7],
      g_SI4463ItStatus[8],
      SI446x_Get_Device_Status());
      Delay_ms(250);
    }

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
