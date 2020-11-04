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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DELAY/delay.h"
#include "OLED/oled_iic.h"
#include "GRIDKEY/gridkey.h"
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
    uint16_t pinList[16]={GPIO_PIN_9, GPIO_PIN_9, GPIO_PIN_7, GPIO_PIN_15, GPIO_PIN_13, GPIO_PIN_11, GPIO_PIN_9, GPIO_PIN_15, GPIO_PIN_10, GPIO_PIN_8, GPIO_PIN_8, GPIO_PIN_6, GPIO_PIN_14, GPIO_PIN_12, GPIO_PIN_10, GPIO_PIN_8};
    GPIO_TypeDef* grpList[16]={GPIOA, GPIOC, GPIOC, GPIOD, GPIOD, GPIOD, GPIOD, GPIOB, GPIOA, GPIOA, GPIOC, GPIOC, GPIOD, GPIOD, GPIOD, GPIOD};

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
  /* USER CODE BEGIN 2 */
    Delay_Init(168);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,GPIO_PIN_RESET);
    uint8_t pinAState=0,thState=0,deltaState=0;
    uint8_t isEditingDelta=0;
    float threshold=37;
    float delta=1.5;
    OLED_Init();
    OLED_ShowStr(0,0,"Temperature:",TEXTSIZE_SMALL,REVERSE_OFF);
    OLED_ShowStr(0,2,"Threshold:",TEXTSIZE_SMALL,REVERSE_OFF);
    OLED_ShowFloat(0,3,threshold,TEXTSIZE_SMALL,REVERSE_OFF);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      uint8_t key=GridKey_ScanRaw(0);
      
      if(key!=255 && key!=3 && key!=7 && key!=11 && key!=15)
      //if(key!=255)
      {
          HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9,((key>>0)&1u));
          HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11,((key>>1)&1u));
          HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,((key>>2)&1u));
          HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,((key>>3)&1u));
          HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_SET);
      }
      else
          HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_RESET);
      if(key==3 && pinAState==0)
      {
          HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_0);
          pinAState=1;
      }
      else if(key!=3)
      {
          pinAState=0;
      }
          
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2));
      
      uint16_t rawTemp=0;
      for(int i=0;i<16;i++)
      {
          rawTemp|=(HAL_GPIO_ReadPin(grpList[i],pinList[i])<<i);
      }
      float Temp=(float)rawTemp*0.02-273.15;
      Temp+=delta;
      if(Temp>=35&&Temp<=40)
          Temp+=0.5;
      else if(Temp>40)
          Temp+=1;
      OLED_ShowFloat(0,1,Temp,TEXTSIZE_SMALL,REVERSE_OFF);
      
      if((key==7 || key==11) && thState==0)
      {
          if(isEditingDelta)
          {
              if(key==7 && delta<4)
                  delta+=0.25;
              else if(key==11 && delta>-2)
                  delta-=0.25;
              thState=1;
              OLED_ShowFloat(0,5,delta,TEXTSIZE_SMALL,REVERSE_OFF);
              Delay_ms(20);
              OLED_ShowStr(0,5,"        ",TEXTSIZE_SMALL,REVERSE_OFF);
          }
          else
          {
              if(key==7 && threshold<47)
                  threshold+=0.5;
              else if(key==11 && threshold>29)
                  threshold-=0.5;
              thState=1;
              OLED_ShowFloat(0,3,threshold,TEXTSIZE_SMALL,REVERSE_OFF);
          }
      }
      else if(key!=7 && key!=11)
      {
          thState=0;
      }
      
      if(key==15 && deltaState==0)
      {
          isEditingDelta=!isEditingDelta;
          deltaState=1;
      }
      else if(key!=15)
      {
          deltaState=0;
      }
      
      HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1,!(Temp>threshold));
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
