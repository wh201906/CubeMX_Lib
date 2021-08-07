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
#include "MPU/mpu6050.h"
#include "MPU/MD6.12/core/driver/eMPL/inv_mpu.h"
#include "MPU/MD6.12/core/mllite/mpl.h"
#include "MPU/MD6.12/core/mpl/quaternion_supervisor.h"
#include "MPU/MD6.12/core/mpl/gyro_tc.h"
#include "MPU/MD6.12/core/eMPL-hal/eMPL_outputs.h"
#include "MPU/MD6.12/core/mpl/fast_no_motion.h"
#include "MPU/MD6.12/core/mpl/fast_no_motion.h"
#include "MPU/MD6.12/core/mpl/fast_no_motion.h"
#include "MPU/MD6.12/core/mllite/data_builder.h"
#include "MPU/MD6.12/core/mllite/ml_math_func.h"
#include "MPU/MD6.12/core/driver/eMPL/inv_mpu_dmp_motion_driver.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)
#define COMPASS_ON      (0x04)
#define DEFAULT_MPU_HZ (20)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
MyUARTHandle uart1;
uint8_t uartBuf1[100];
uint16_t i2cAddr[10];
extern SoftI2C_Port MPU6050_Port;

struct platform_data_s {
    signed char orientation[9];
};
static struct platform_data_s gyro_pdata = {
    .orientation = { 1, 0, 0,
                     0, 1, 0,
                     0, 0, 1}
};
struct rx_s {
    unsigned char header[3];
    unsigned char cmd;
};
struct hal_s {
    unsigned char lp_accel_mode;
    unsigned char sensors;
    unsigned char dmp_on;
    unsigned char wait_for_tap;
    volatile unsigned char new_gyro;
    unsigned char motion_int_mode;
    unsigned long no_dmp_hz;
    unsigned long next_pedo_ms;
    unsigned long next_temp_ms;
    unsigned long next_compass_ms;
    unsigned int report;
    unsigned short dmp_features;
    struct rx_s rx;
};
static struct hal_s hal = {0};
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
  uint32_t i, tmp;
  uint8_t data;
  uint8_t accel_fsr,  new_temp = 0;
  uint16_t gyro_rate, gyro_fsr;
  int16_t gyro[3], accel[3];
  int32_t quat[4];
  
  uint32_t sensor_timestamp;
  int16_t sensors;
  uint8_t more;
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
  SoftI2C_SetPort(&MPU6050_Port, GPIOE, 2, GPIOE, 3);
  SoftI2C_Init(&MPU6050_Port,100000,SI2C_ADDR_7b);
  printf("MPU Test\r\n");
  
  // find available device
  printf("available address: \r\n");
  Delay_ms(1000);
  tmp = SoftI2C_SearchAddr(&MPU6050_Port, 0x00, 0xFF, i2cAddr);
  for(i = 0; i < tmp; i++)
  {
    printf("0x%x ", i2cAddr[i]);
  }
  printf("\r\n");
  
  // read whoami register
  SoftI2C_Read(&MPU6050_Port, MPU6050_I2C_ADDR, 0x75, &data, 1);
  printf("whoami: 0x%x\r\n", data);
  printf("mpu_init(): %d\r\n", mpu_init(NULL));
  printf("inv_init_mpl(): %d\r\n", inv_init_mpl());
  printf("inv_enable_quaternion(): %d\r\n", inv_enable_quaternion());
  // printf("inv_enable_9x_sensor_fusion(): %d\r\n", inv_enable_9x_sensor_fusion());
  printf("inv_enable_fast_nomot(): %d\r\n", inv_enable_fast_nomot());
  printf("inv_enable_gyro_tc(): %d\r\n", inv_enable_gyro_tc());
  printf("inv_enable_eMPL_outputs(): %d\r\n", inv_enable_eMPL_outputs());
  printf("mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL): %d\r\n", mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL));
  printf("inv_start_mpl(): %d\r\n", inv_start_mpl());
  printf("mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL): %d\r\n", mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL));
  printf("mpu_set_sample_rate(): %d\r\n", mpu_set_sample_rate(DEFAULT_MPU_HZ));
  mpu_get_sample_rate(&gyro_rate);
  mpu_get_gyro_fsr(&gyro_fsr);
  mpu_get_accel_fsr(&accel_fsr);
  printf("gyro_rate: %u\r\ngyro_fsr: %u\r\naccel_fsr: %u\r\n", gyro_rate, gyro_fsr, accel_fsr);
  inv_set_gyro_sample_rate(1000000L / gyro_rate);
  inv_set_accel_sample_rate(1000000L / gyro_rate);
  inv_set_gyro_orientation_and_scale(inv_orientation_matrix_to_scalar(gyro_pdata.orientation), (long)gyro_fsr<<15);
  inv_set_accel_orientation_and_scale(inv_orientation_matrix_to_scalar(gyro_pdata.orientation), (long)accel_fsr<<15);
  hal.sensors = ACCEL_ON | GYRO_ON;
  hal.dmp_on = 0;
  hal.report = 0;
  hal.rx.cmd = 0;
  hal.next_pedo_ms = 0;
  hal.next_compass_ms = 0;
  hal.next_temp_ms = 0;
  // get_tick_count(&timestamp);
  printf("dmp_load_motion_driver_firmware(): %d\r\n", dmp_load_motion_driver_firmware());
  printf("dmp_set_orientation(): %d\r\n", dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_pdata.orientation)));
  // dmp_register_tap_cb(tap_cb);
  // dmp_register_android_orient_cb(android_orient_cb);
  hal.dmp_features = DMP_FEATURE_6X_LP_QUAT |
        DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
        DMP_FEATURE_GYRO_CAL;
  printf("dmp_enable_feature(): %d\r\n", dmp_enable_feature(hal.dmp_features));
  printf("dmp_set_fifo_rate(): %d\r\n", dmp_set_fifo_rate(DEFAULT_MPU_HZ));
  printf("mpu_set_dmp_state(): %d\r\n", mpu_set_dmp_state(1));
  hal.dmp_on = 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    Delay_ms(100);
    mpu_get_gyro_reg(gyro, NULL);
    mpu_get_accel_reg(accel, NULL);
    printf("Raw:\r\n");
    printf("gyro: %d, %d, %d\r\n", gyro[0], gyro[1], gyro[2]);
    printf("accel: %d, %d, %d\r\n", accel[0], accel[1], accel[2]);
    dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);
    if(!more)
      continue;
    printf("DMP:\r\n");
    printf("gyro: %d, %d, %d\r\n", gyro[0], gyro[1], gyro[2]);
    printf("accel: %d, %d, %d\r\n", accel[0], accel[1], accel[2]);
    printf("quat: %d, %d, %d, %d\r\n", quat[0], quat[1], quat[2], quat[3]);
    printf("sensors: 0x%x\r\n", sensors);
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
