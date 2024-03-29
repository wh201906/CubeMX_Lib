/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DELAY/delay.h"
#include "SIGNAL/myfft.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FFT_LENGTH 1024

uint32_t sampleRate=2000000;
void printAll(float32_t* addr,uint16_t len);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t val[FFT_LENGTH];
float32_t fftData[FFT_LENGTH];
MyUARTHandle myuart1, myuart2;
uint8_t uartBuf1[100], uartBuf2[100];
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
  char str[20];
  uint16_t i,j;
  uint8_t outVal;
  float32_t tmp;
  volatile uint8_t st[12];
  char hexTable[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  Delay_Init(120);
  HAL_ADC_Start_DMA(&hadc1,(uint32_t*)val,FFT_LENGTH);
  HAL_TIM_Base_Start(&htim2);
  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);
  MyUART_Init(&myuart1, USART1, uartBuf1, 100);
  MyUART_Init(&myuart2, USART2, uartBuf2, 100);
  MyUART_SetOStream(USART1);
  Delay_ms(500);
  HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_4);
  MyFFT_Init(sampleRate);
  MyFFT_FlattopWindow();
  HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_4);
  Delay_ms(200);
  //printAll(rfftInAndFreq,FFT_LENGTH/2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(__HAL_ADC_GET_FLAG(&hadc1,ADC_FLAG_OVR))
    {
      hadc1.Instance->CR2 &= ~ADC_CR2_DMA;
      HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_4);
      for(i=0;i<FFT_LENGTH;i++)
        fftData[i]=val[i];
      HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_4);
      MyFFT_CalcInPlace(fftData);
      HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_4); 
      //printAll(fftData,sizeof(fftData)/sizeof(fftData[0]));
      
      // Init transfer
      MyUART_ClearBuffer(&myuart2);
      str[0]=0x00;
      j=0;
      
      // Write command
      MyUART_WriteStr(&myuart2, "ref_stop\xFF\xFF\xFF");
      Delay_ms(5);
      MyUART_WriteStr(&myuart2, "cle 1,0\xFF\xFF\xFF");
      Delay_ms(5);
      MyUART_WriteStr(&myuart2, "addt 1,0,480\xFF\xFF\xFF");
      
       // Wait for response in 30ms
      for(i=0;i<15;i++)
      {
        if(str[0]==0xFE)
          break;
        Delay_ms(2);
        j=MyUART_Read(&myuart2, str + j, 4);
      }
      arm_max_no_idx_f32(fftData,FFT_LENGTH/2,&tmp);
      tmp=256/tmp;
      
      // Write data
      for(i=0;i<480;i++)
      {
        outVal=fftData[i]*tmp;
        MyUART_WriteChar(&myuart2, outVal);
      }
      Delay_ms(5);
      MyUART_WriteStr(&myuart2, "ref_star\xFF\xFF\xFF");
      Delay_ms(5);
      MyUART_ClearBuffer(&myuart2);
      // DMA DOES take the bandwidth, so I start it after UART transmition.
      HAL_ADC_Start_DMA(&hadc1,(uint32_t*)val,FFT_LENGTH);
    }
    for(i=0;i<10;i++)
    {
      Delay_ms(30);
      j=MyUART_Read(&myuart2, str, 3);
      if(j==3)
      {
        sampleRate = str[0]&0xFF;
        sampleRate |= (uint32_t)(str[1]<<8);
        sampleRate |= (uint32_t)(str[2]<<16);
        // for a 32-bit ARR register, 
        // the prescaler can be set to 0, 
        // and the lowest freq is 168M / (2^32-1) = 0.04Hz
        // or in this program: 60M / (2^32-1) = 0.014Hz
        
        // PRESCALER has been set to 0
        __HAL_TIM_SET_AUTORELOAD(&htim2,(60000000.0/2.0/sampleRate+0.5)-1);
        
      }
    }
    
    MyUART_ClearBuffer(&myuart2);
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
  RCC_OscInitStruct.PLL.PLLN = 120;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void printAll(float32_t* addr,uint16_t len)
{
  char buf[20];
  uint16_t i;
  for(i=0;i<len;i++)
  {
    sprintf(buf,"%d:%f",i,*(addr+i));
    MyUART_WriteLine(&myuart1, buf);
    Delay_ms(2);
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
