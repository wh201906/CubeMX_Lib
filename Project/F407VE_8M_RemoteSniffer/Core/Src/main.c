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
#define RECVLEN 32
#define TRIGTHRE 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
MyUARTHandle uart1;
uint8_t uartBuf1[100];
uint8_t rId = 0;
uint16_t dId = 0;
uint8_t remoteBuf[4];
uint64_t decodeBuf[RECVLEN];
uint8_t lastData = 0;
uint8_t trigCnt = 0;
uint8_t triggered = 0; // 0:untriggered, 1:triggered, 2:triggered with output
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  uint8_t data, tmp;
  if (GPIO_Pin != SI4463_GPIO0_PIN || !SI4463_GPIO0_R())
    return;
  remoteBuf[rId++] = SI4463_GPIO1_R();
  rId %= 4;
  if(rId == 0)
  {
    data = remoteBuf[0] << 3 | remoteBuf[1] << 2 | remoteBuf[2] << 1 | remoteBuf[3];
    data &= 0xF;
    if(lastData == 0x0) // alignment
    {
      if(data == 0x1) // 0000 0001 xxxx->0000 1xxx
      {
        remoteBuf[0] = 1;
        rId = 1;
        return;
      }
      else if(data == 0x2 || data == 0x3) // 0000 001x xxxx->0000 1xxx
      {
        remoteBuf[0] = 1;
        remoteBuf[1] = remoteBuf[3];
        rId = 2;
        return;
      }
      else if(data == 0x4 || data == 0x7) // 0000 01xx xxxx->0000 1xxx
      {
        remoteBuf[0] = 1;
        remoteBuf[1] = remoteBuf[2];
        remoteBuf[2] = remoteBuf[3];
        rId = 3;
        return;
      }
      lastData = data;
    }
    else if(lastData == 0xF)
    {
      if(data == 0x1 || data == 0xD) // 1111 0001/1111 1101->1111 1000 1xxx/1111 1110 1xxx
      {
        data = (data & 0xF) >> 1 | 0x8;
        remoteBuf[0] = 1;
        rId = 1;
      }
      else if(data == 0xA || data == 0xB) // 1111 101x->1111 1110 1xxx
      {
        data = 0xE;
        remoteBuf[0] = 1;
        remoteBuf[1] = remoteBuf[3];
        rId = 2;
      }
      else if(data == 0x4 || data == 0x7) // 1111 01xx->1111 1110 1xxx
      {
        data = 0xE;
        remoteBuf[0] = 1;
        remoteBuf[1] = remoteBuf[2];
        remoteBuf[2] = remoteBuf[3];
        rId = 3;
      }
      else if(data == 0xC) // 1111 1100 01xx->1111 1000 1xxx
      {
        data = 0x8;
        remoteBuf[0] = 1;
        remoteBuf[1] = remoteBuf[2];
        remoteBuf[2] = remoteBuf[3];
        rId = 3;
      }
      lastData = data;
    }
    // TODO:(need post process/overwrite)
    // 1111 1110 001x->1111 1000 1xxx(lastData=0xF, data=0xE, nextData=0x2/0x3)
    else
    {
      lastData = data;
    }
    tmp = dId % 16;
    if(tmp == 0)
      decodeBuf[dId / 16] = 0;
    decodeBuf[dId / 16] |= (uint64_t)data << ((dId % 16) * 4);
    dId++;
    dId %= RECVLEN * 16;
  }
  if((data == 0x8 || data == 0xE) && trigCnt <= TRIGTHRE)
  {
    trigCnt++;
  }
  if(triggered == 0 && trigCnt > TRIGTHRE) // trigger threshold
  {
    trigCnt = 0;
    triggered = 1;
  }
  if(dId == 0 && triggered == 2)
    triggered = 0;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t i;
  uint8_t curr;
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
  printf("Si4463 Remote Sniffer Test\r\n");
  HAL_NVIC_DisableIRQ(EXTI0_IRQn);
  SI446x_Init();
  
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    Delay_ms(200);
    if(triggered != 1)
      continue;
    uint8_t* ptr = decodeBuf; // %x is not compatible with uint64_t
    for(i = 1; i <= RECVLEN * 8; i++)
    {
      curr = *(ptr++);
      printf("%x%x", curr&0xF, curr>>4);
      if(i % 4 == 0)
        printf(" ");
      if(i % 16 == 0)
        printf("\r\n");
    }
    printf("\r\n-------------------------\r\n");
    triggered = 2;
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
