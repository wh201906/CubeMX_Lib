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
#include "arm_math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_LEN 4096
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
MyUARTHandle uart1;
uint8_t uartBuf1[100];
uint16_t adcBuf[ADC_LEN];
uint16_t adcBuf2[ADC_LEN];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// for LF, set filter to 200
// for HF, reduce filter value
int32_t phaseDetect(uint32_t len, uint8_t filter)
{
  uint16_t mean1, mean2;
  uint32_t i, z1 = 0, z2 = 0;
  // find virtual zero, ignore the different amplitude
  arm_mean_q15(adcBuf, len, &mean1);
  arm_mean_q15(adcBuf2, len, &mean2);
  // find zero-crossing
  //printf("mean1:%d\r\n", mean1);
  for(i = 0; i < len - 1; i++)
  {
    if((adcBuf[i] - mean1 & 0x8000) && !(adcBuf[i+1] - mean1 & 0x8000)) // posedge
    {
      // filter
      // the adcBuf contains at least 1 complete period
      if(i > filter && adcBuf[i-filter] > adcBuf[i])
        continue;
      else if(i < len - filter && adcBuf[i+filter] < adcBuf[i])
        continue;
        
      z1 = i;
      //printf("i:%d,1[i]:%d,1[i+1]:%d\r\n", i, adcBuf[i] & 0xFFF, adcBuf[i+1] & 0xFFF);
      break;
    }
  }
  //printf("mean2:%d\r\n", mean2);
  for(i = 0; i < len - 1; i++)
  {
    if((adcBuf2[i] - mean2 & 0x8000) && !(adcBuf2[i+1] - mean2 & 0x8000)) // posedge
    {
      if(i > filter && adcBuf2[i-filter] >= adcBuf2[i])
        continue;
      else if(i < len - filter && adcBuf2[i+filter] <= adcBuf2[i])
        continue;
      
      z2 = i;
      //printf("i:%d,2[i]:%d,2[i+1]:%d\r\n", i, adcBuf2[i] & 0xFFF, adcBuf2[i+1] & 0xFFF);
      break;
    }
  }
  return (int32_t)z1 - (int32_t)z2;
}

uint32_t measure(double freq, uint16_t* amp1, uint16_t* amp2, int32_t* phase)
{
  uint8_t phaseFilter = 0;
  uint32_t len, tmp, i;
  uint16_t maxVal, minVal;
  if(freq < 150000 && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) != GPIO_PIN_SET)
  {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
    Delay_ms(50);
  }
  else if(freq >= 150000 &&  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) != GPIO_PIN_RESET)
  {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
    Delay_ms(50);
  }
  // HF: sample for 50 periods
  tmp = 2; // 168M / ((arr + 1) * (psc + 1)) = 14M < 0.25APB2 clock speed(21M)
  htim8.Instance->PSC = tmp;
  tmp = 168000000 / (__HAL_TIM_GET_AUTORELOAD(&htim8) + 1) / (tmp + 1); // current sample rate
  tmp /= (uint32_t)freq; // dot per period
  
  if(tmp > ADC_LEN / 5)
    phaseFilter = 200;
  
  len = tmp * 50; // dot for 50 periods
  if(len > ADC_LEN)
    len = ADC_LEN;
  
  // LF: sample for 1 period
  // for sine wave:
  // slope_max = max(sin'(x)) = Vpp
  // x_step: 1 / ADC_LEN
  // offset_max < slope_max * x_step
  // offset_max < Vpp / ADC_LEN
  if(tmp > ADC_LEN)
  {
    tmp = freq * ADC_LEN - 1; // required sample rate
    tmp = 168000000 / tmp + 1; // (arr + 1) * (psc + 1)
    tmp /= (__HAL_TIM_GET_AUTORELOAD(&htim8) + 1); // psc + 1
    htim8.Instance->PSC = tmp;
  }
  
  AD9834_SetFreq(freq, 0);
  Delay_us(1);
  htim8.Instance->EGR = TIM_EGR_UG; // update arr and psc
  ParaIO_Start_In_Sync(adcBuf, len, adcBuf2, len);
  while(!ParaIO_IsTranferCompleted_In2())
    ;
  
  // the width of adc result is 12bit. So the MSB of uint16_t is always zero.
  // q15 works there.
  
  // for the second AD9226:
  // D11~D0 -> PE13~PE2
  arm_shift_q15(adcBuf2, -2, adcBuf2, len);
  for(i = 0; i < len; i++)
  {
    adcBuf[i] &= 0xFFF;
    adcBuf2[i] &= 0xFFF;
  }
  arm_max_q15(adcBuf, len, &maxVal, &tmp);
  arm_min_q15(adcBuf, len, &minVal, &tmp);
  *amp1 = maxVal - minVal;
  arm_max_q15(adcBuf2, len, &maxVal, &tmp);
  arm_min_q15(adcBuf2, len, &minVal, &tmp);
  *amp2 = maxVal - minVal;
  *phase = phaseDetect(len, phaseFilter);
  return len;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  double freq;
  uint16_t i, amp1, amp2;
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
  /* USER CODE BEGIN 2 */
  Delay_Init(168);
  MyUART_Init(&uart1, USART1, uartBuf1, 100);
  //printf("Sweep Test\r\n");
  AD9834_Init(&hspi1);
  AD9834_SelectReg(0, 0);
  AD9834_SetWaveType(AD9834_Sine, AD9834_SOff);
  
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
  ParaIO_Init_In(&htim8, 16, 0);
  ParaIO_Init_In2(&htim8, 16, 0);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    freq = 30;
//    tmp = measure(freq, &amp1, &amp2, &phase);
//    if(phase < 500 && phase > -500)
//      continue;
//    for(i = 0; i<tmp; i++)
//      printf("%d,%d,%d\r\n", adcBuf[i] & 0xFFF, adcBuf2[i] & 0xFFF, phase);
    
    for(; freq < 200; freq += 10)
    {
      measure(freq, &amp1, &amp2, &phase);
      printf("%d,%d,%d\r\n", amp1 & 0xFFF, amp2 & 0xFFF, phase);
    }
    for(; freq < 5000; freq += 100)
    {
      measure(freq, &amp1, &amp2, &phase);
      printf("%d,%d,%d\r\n", amp1 & 0xFFF, amp2 & 0xFFF, phase);
    }
    for(; freq <= 2000000; freq += 50000) // if the period of this loop is too long, the result will be wrong
    {
      measure(freq, &amp1, &amp2, &phase);
      printf("%d,%d,%d\r\n", amp1 & 0xFFF, amp2 & 0xFFF, phase); // print more characters will cause bugs
      //Delay_ms(1); un-comment it will cause bugs
    }
    Delay_ms(1000);
    
    
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
