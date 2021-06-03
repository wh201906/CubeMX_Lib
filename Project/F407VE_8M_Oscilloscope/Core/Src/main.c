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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DELAY/delay.h"
#include "LCD/lcd.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define ARRLEN 512
#define DISPLAYLEN 320
#define THRESHOLD 2048
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t val[ARRLEN];
uint16_t waveBuf1[DISPLAYLEN * 2], waveBuf2[DISPLAYLEN * 2];
uint8_t waveState = 0;
double scaler = 0.07;
uint8_t displayMode = 1; // 0: dot, 1: vector
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Wave_DrawLine(int16_t* lastBuf, int16_t* currBuf, uint16_t len, uint32_t backColor, uint32_t color)
{
  uint16_t i, j;
  for(i = 0; i<len;i++)
  {
    for(j = lastBuf[2*i]; j<=lastBuf[2*i+1]; j++)
      LCD_Fast_DrawPoint(j, i, backColor);
    for(j = currBuf[2*i]; j<=currBuf[2*i+1]; j++)
      LCD_Fast_DrawPoint(j, i, color);
  }
}

void Wave_CalcLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, int16_t* buf)
{
  uint16_t t;
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, uRow, uCol;
  delta_x = x2 - x1; //计算坐标增量
  delta_y = y2 - y1;
  uRow = x1;
  uCol = y1;
  if (delta_x > 0)
    incx = 1; //设置单步方向
  else if (delta_x == 0)
    incx = 0; //垂直线
  else
  {
    incx = -1;
    delta_x = -delta_x;
  }
  if (delta_y > 0)
    incy = 1;
  else if (delta_y == 0)
    incy = 0; //水平线
  else
  {
    incy = -1;
    delta_y = -delta_y;
  }
  if (delta_x > delta_y)
    distance = delta_x; //选取基本增量坐标轴
  else
    distance = delta_y;
  for (t = 0; t <= distance + 1; t++) //画线输出
  {
    
    if(buf[uCol*2]==-1 && buf[uCol*2+1]==-1)
      buf[uCol*2]=buf[uCol*2+1]=uRow;
    else if(uRow<buf[uCol*2])
      buf[uCol*2]=uRow;
    else if(uRow>buf[uCol*2+1])
      buf[uCol*2+1]=uRow;
    
    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance)
    {
      xerr -= distance;
      uRow += incx;
    }
    if (yerr > distance)
    {
      yerr -= distance;
      uCol += incy;
    }
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */
  Delay_Init(168);
  LCD_Init();
  HAL_TIM_Base_Start(&htim2);
  LCD_Clear(WHITE);
  HAL_ADC_Start_DMA(&hadc1, val, ARRLEN);
  Delay_ms(200);
  LCD_SetPointColor(RED);
  LCD_SetBkGNDColor(WHITE);
  Delay_ms(200);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    Delay_ms(200);
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
uint16_t ModCalc(int16_t num)
{
  return ((2 * ARRLEN + num) % ARRLEN);
}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc)
{
  uint16_t start, rest, i, j, lastVal, currVal;
  int16_t *currWave, *lastWave;
  if (hadc == &hadc1)
  {
    __HAL_ADC_DISABLE_IT(hadc, ADC_IT_AWD);
    Delay_ms(50);
    rest = hadc1.DMA_Handle->Instance->NDTR;
    if (hadc1.Instance->HTR == THRESHOLD)
    {
      hadc1.Instance->HTR = 0xFFF;
      hadc1.Instance->LTR = THRESHOLD;

      lastWave = waveState ? waveBuf2 : waveBuf1;
      currWave = waveState ? waveBuf1 : waveBuf2;
      waveState = !waveState;

      __HAL_DMA_DISABLE(&hdma_adc1);
      start = ModCalc(-rest - DISPLAYLEN);
      if (displayMode == 0)
      {
        LCD_SetPointColor(WHITE);
        for (i = 0; i < DISPLAYLEN; i++)
          LCD_DrawPoint(lastWave[i], i);
        for (i = 0; i < DISPLAYLEN; i++)
          currWave[i] = (uint16_t)(val[(start + i) % ARRLEN] * scaler);
        LCD_SetPointColor(RED);
        for (i = 1; i < DISPLAYLEN; i++)
          LCD_DrawPoint(currWave[i], i);
      }
      else
      {
        memset(currWave, -1, sizeof(waveBuf1));
        lastVal = val[(start + 0) % ARRLEN] * scaler;
        for (i = 1; i < DISPLAYLEN; i++)
        {
          currVal = val[(start + i) % ARRLEN] * scaler;
          Wave_CalcLine(currVal, i, lastVal, i - 1, currWave);
          lastVal=currVal;
        }
        LCD_SetPointColor(RED);
        Wave_DrawLine(lastWave, currWave, DISPLAYLEN, WHITE, RED);
      }
      __HAL_DMA_ENABLE(&hdma_adc1);
    }
    else
    {
      hadc1.Instance->HTR = THRESHOLD;
      hadc1.Instance->LTR = 0;
    }
    __HAL_ADC_ENABLE_IT(hadc, ADC_IT_AWD);
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
