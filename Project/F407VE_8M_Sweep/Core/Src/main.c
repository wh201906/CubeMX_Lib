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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "AD9834/ad9834.h"
#include "PARAIO/paraio.h"
#include "sweep.h"
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
MyUARTHandle uart1;
uint8_t uartBuf1[100];
uint16_t adcBuf[ADC_LEN + ADC_PIPELINE_DELAY];
uint16_t adcBuf2[ADC_LEN + ADC_PIPELINE_DELAY];
double freq[SWEEP_LEN_MAX], amp[SWEEP_LEN_MAX], ph[SWEEP_LEN_MAX];
double damp[SWEEP_LEN_MAX];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  double currFreq;
  uint32_t i, j;
  uint16_t amp1, amp2;
  int32_t phase;
  uint32_t tmp;
  
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
  MX_TIM8_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  Delay_Init(168);
  MyUART_Init(&uart1, USART1, uartBuf1, 100);
  //printf("Sweep Test\r\n");
  
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  LED_off();
  
  AD9834_Init(&hspi1);
  AD9834_SelectReg(0, 0);
  AD9834_SetWaveType(AD9834_Sine, AD9834_SOff);
  
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
  ParaIO_Init_In(&htim8, 16, 0);
  ParaIO_Init_In2(&htim8, 16, 0);
  
  calibrate(30, UBAND_THRE);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    currFreq = 30;
//    tmp = measure(currFreq, &amp1, &amp2, &phase);
//    if(phase < 500 && phase > -500)
//      continue;
//    for(i = 0; i<tmp; i++)
//      printf("%d,%d,%d\r\n", adcBuf[i] & 0xFFF, adcBuf2[i] & 0xFFF, phase);
    i = 0;
    j = 0;
    tmp = sweep(30, UBAND_THRE, NULL);
    
    double mmin=amp[0];
    double mmax=amp[0];
    double ave=0;
    double ep=0;
    
    
    for(i = 0; i < tmp; i++)
    {
      mmin = amp[i] < mmin ? amp[i] : mmin;
      mmax = amp[i] > mmax ? amp[i] : mmax;
      ave += amp[i];
      if(fabs(amp[i] - 0.70711) < fabs(amp[j] - 0.70711))
      {
        //printf("update:%f,%f\r\n", freq[i], amp[i]);
        j = i;
      }
    }
    ave /= tmp;
    
    if(ave - mmin < 0.1 || mmax - ave < 0.1) // R / Open / Short
    {
      printf("%d,%f\r\n", i, ave);
      if(ave < 0.08)
      {
        LED_fast();
        printf("Short\r\n");
      }
      else if(ave > 0.97)
      {
        LED_slow();
        printf("Open\r\n");
      }
      else
      {
        LED_off();
        printf("R:%f\r\n", calc_R(ave));
      }
      
    }
    else // L/C
    {
      LED_off();
      // edge
      if(j == 0)
        ep = freqSearch(freq[0], freq[j+1], amp[0], amp[j+1], 0.70711, 0.001);
      else if(j == tmp - 1)
        ep = freqSearch(freq[j-1], freq[j], amp[j-1], amp[j], 0.70711, 0.001);
      else
        ep = freqSearch(freq[j-1], freq[j+1], amp[j-1], amp[j+1], 0.70711, 0.001);
      
      printf("%d,%f,%f\r\n", i, ave, ep);
      if(amp[5] > amp[tmp - 5])
        printf("C:%f\r\n", calc_C(ep));
      else
        printf("L:%f\r\n", calc_L(ep));
    }
    
    /*
    for(i = 0; i < tmp; i++)
    {
      printf("-%d,%f,%f\r\n", (uint32_t)freq[i], amp[i], ph[i]);
    }
    Delay_ms(500);
    */
    
    
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
