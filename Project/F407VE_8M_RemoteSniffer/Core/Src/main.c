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
#include "UTIL/mygpio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RECVLEN 32
#define TRIGTHRE 16
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
uint16_t trigCnt = 0;
uint8_t trigSt = 0; // 0: untriggered, 1: acquire, 2: stop
uint8_t threCnt = 0;
char hexTable[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
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
    if(trigSt == 2)
      return;
    data = remoteBuf[0] << 0 | remoteBuf[1] << 1 | remoteBuf[2] << 2 | remoteBuf[3] << 3;
    data &= 0xF;
    tmp = dId % 16;
    if(tmp == 0)
      decodeBuf[dId / 16] = 0;
    decodeBuf[dId / 16] |= (uint64_t)data << ((dId % 16) * 4);
    dId++;
    dId %= RECVLEN * 16;
  }
  if(trigSt == 0)
  {
    trigCnt = 0;
    if(data!=0x0 && data!= 0xF) // condition
    {
      if(threCnt >= TRIGTHRE)
        trigSt = 1;
      else
        threCnt++;
    }
  }
  else if(trigSt == 1)
  {
    if(trigCnt >= RECVLEN * 8) // half of the buffer
      trigSt = 2;
    else
      trigCnt++;
    threCnt = 0;
  }
}

uint8_t halfByte(uint8_t* start, uint32_t offset) // offset in bits
{
  uint8_t b, tmp;
  b = *(start + offset / 8);
  tmp = offset % 8;
  b >>= tmp;
  if(tmp > 4) // [5,7]
  {
    b &= (uint8_t)-1 >> tmp;
    b |= *(start + offset / 8 + 1) << (8 - tmp);
  }
  b &= 0xF;
  return b;
}

uint32_t pt2262(uint8_t* start, uint8_t* end, uint32_t offset) // offset in bits
{
  uint8_t curr;
  uint32_t i, len;
  if(end < start)
    return 0;
  len = (end - start + 1) * 8;
  i = offset;
  while(i < len) // find head and align
  {
    curr = halfByte(start, i);
    if(curr == 0x1 || curr == 0x7)
      break;
    i++;
  }
  if(i >= len)
    return 0;
  for(; i < len; i+=4) // output the rest
  {
    curr = halfByte(start, i);
    putchar(hexTable[curr]);
    if(curr != 0x1 && curr != 0x7)
      break;
  }
  return i;
}

void Set_GPIO(uint8_t isRx)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  uint8_t seq[8] = {0x13, 0x11, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
  
  if(isRx)
  {
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    seq[2] = 0x14;
  }
  else
  {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  }
  SI446x_Write_Cmds(seq, 8);
  SI446x_Read_Response(seq, 8);
  
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
  MyGPIO_Init(SI4463_GPIO1_PORT, SI4463_GPIO1_PIN, 1);   
}

void SendTest(uint16_t delayus)
{
  uint16_t i, j;
  uint64_t buf[2] = {0x8b8b888b8b8bbbb8, 0x0000000b88888bb8};
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  Set_GPIO(0);
  SI446x_Start_Tx(0, 0, PACKET_LENGTH);
  Delay_ms(120);
  for(i = 0; i < 5; i++)
  {
    for(j = 0; j < 124; j++)
    {
      SI4463_GPIO1_W(halfByte(buf, j) & 1);
      Delay_us(delayus);
    }
  }
  Delay_ms(1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t i, j;
  uint16_t* endPtr;
  uint64_t curr;
  uint16_t* ptr;
  uint32_t offset;
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
  //HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  
  Delay_ms(2000);
  
  SendTest(460);
  //HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_4);
  //HAL_NVIC_DisableIRQ(EXTI0_IRQn);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //Delay_ms(20);
    if(trigSt != 2)
      continue;
    for(endPtr = ((uint16_t*)&decodeBuf[RECVLEN - 1] + 3); endPtr > (uint16_t*)decodeBuf; endPtr--)
    {
      if(*endPtr != (uint16_t)-1 && *endPtr != 0)
        break;
    }
    ptr = (uint16_t*)decodeBuf; // %x is not compatible with uint64_t
    while(*ptr == (uint16_t)-1 || *ptr == 0)
      ptr++;
    if(ptr > (uint16_t*)decodeBuf)
      ptr--;
    offset = 0;
    while((offset = pt2262((uint8_t*)ptr, ((uint8_t*)endPtr + 1), offset)) != 0)
      printf("\r\n");
    printf("*******\r\n");
    
    for(i = 0; i < RECVLEN * 4; i++)
    {
      if(ptr > endPtr)
        break;
      curr = *(ptr++);
      for(j = 0; j < 4; j++)
      {
        putchar(hexTable[curr >> 4*j & 0xF]);
        if((j+1)%4==0)
          putchar(' ');
      }
      if((i+1)%4==0)
        printf("\r\n");
    }
    printf("\r\n-------------------------\r\n");
    trigSt = 0;
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
