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
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "AD9910/ad9910.h"
#include "arm_math.h"
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
MyUARTHandle uart1, uart2;
uint8_t uartBuf1[100], uartBuf2[100];
uint16_t valk[15], valm[420];
uint16_t maxX, threX;
double tmp;
double delta;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SendCMD(char* str)
{
  MyUART_WriteStr(&uart2, str);
  MyUART_WriteStr(&uart2, "\xFF\xFF\xFF");
}

uint8_t WaitResponse(uint8_t ch, uint16_t timeout)
{
  uint8_t rxBuf[15];
  rxBuf[0] = 0x00;
  while (timeout--)
  {
    if (rxBuf[0] == ch)
      return 1;
    Delay_ms(1);
    MyUART_Read(&uart2, rxBuf, 4);
  }
  return 0;
}

void sweep()
{
  uint32_t i, j;
  uint8_t str[20];
  for(i = 1; i <= 10; i++)
  {
    Amp_convert(120);
    Freq_convert(i*100);
    Delay_ms(500);
    HAL_ADC_Start(&hadc1);
    Delay_us(1);
    valk[i] = HAL_ADC_GetValue(&hadc1) / 9;
  }
  for(i = 1; i <= 400; i++)
  {
    Amp_convert(120);
    Freq_convert(i*2500);
    Delay_ms(10);
    HAL_ADC_Start(&hadc1);
    Delay_us(1);
    valm[i] = HAL_ADC_GetValue(&hadc1) / 9;
  }
  for(i = 1; i <= 10; i++)
  {
    printf("%d, %d\r\n", i*100, valk[i]);
  }
  for(i = 1; i <= 400; i++)
  {
    printf("%d, %d\r\n", i*2500, valm[i]);
  }
  SendCMD("ref_stop");
  
  SendCMD("cle 1,0");
  SendCMD("addt 1,0,360");
  WaitResponse(0xFE, 30);
  for(i = 1;i < 10; i++)
  {
    delta = valk[i+1] - valk[i];
    delta /= 40;
    for(j = 0;j < 40; j++)
    {
      MyUART_WriteChar(&uart2, valk[i] + delta * j + 0.5);
    }
  }
  for (i = 0; i < 400; i++)
  {
    tmp = valk[i / 40 + 2] - valk[i / 40 + 1];
    tmp *= (i - (i/40*40)) / 40.0 + 0.5;
    tmp += valk[i / 40 + 1];
    MyUART_WriteChar(&uart2, tmp);
  }
  WaitResponse(0xFD, 30);
  
  SendCMD("cle 2,0");
  SendCMD("addt 2,0,400");
  WaitResponse(0xFE, 30);
  for (i = 1; i <= 400; i++)
    MyUART_WriteChar(&uart2, valm[i]);
  WaitResponse(0xFD, 30);

  SendCMD("ref_star");
  
  maxX = 1;
  for(i = 1;i<=400;i++)
  {
    if(valm[i]> valm[maxX])
      maxX = i;
  }
  printf("max: %d, %d\r\n", maxX, maxX * 2500);
  tmp = valm[maxX];
  tmp *=9*3.3;
  tmp /= 4095;
  tmp *= 4.25; 
  tmp *=1000;
  tmp /=23.72;
  printf("maxVal: %f\r\n", tmp);
  for(i = maxX;i<=400;i++)
  {
    if(valm[i] <= valm[maxX] * 0.707)
    {    
      threX = i;
      break;
    }
  }
  printf("thre: %d, %d\r\n", threX, threX * 2500);
  // thre
  MyUART_WriteStr(&uart2, "t7.txt=\"");
  myitoa(threX * 2500, str, 10);
  MyUART_WriteStr(&uart2, str);
  SendCMD("\"");
  // amp
  MyUART_WriteStr(&uart2, "t6.txt=\"");
  myitoa(tmp, str, 10);
  MyUART_WriteStr(&uart2, str);
  SendCMD("\"");
}
void ImpeO()
{
  uint32_t tmp0, tmp1, val;
  uint8_t str[20];
  Amp_convert(120);
  Freq_convert(1000);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
  Delay_ms(200);
  HAL_ADC_Start(&hadc2);
  Delay_us(1);
  tmp0 = HAL_ADC_GetValue(&hadc2);
 
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
  Delay_ms(200);
  HAL_ADC_Start(&hadc2);
  Delay_us(1);
  tmp1 = HAL_ADC_GetValue(&hadc2);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
  val = (double)(tmp0 - tmp1) / tmp1 * 1600.0;
  // ImpeO
  MyUART_WriteStr(&uart2, "t5.txt=\"");
  myitoa(val, str, 10);
  MyUART_WriteStr(&uart2, str);
  SendCMD("\"");
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
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_ADC2_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
  Delay_Init(168);
  MyUART_Init(&uart1, USART1, uartBuf1, 100);
  MyUART_Init(&uart2, USART2, uartBuf2, 100);
  Init_ad9910();
  Amp_convert(120);
  Freq_convert(1000);
  i = 100;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
    Delay_ms(200);
    sweep();
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
    Delay_ms(200);
    ImpeO();
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
