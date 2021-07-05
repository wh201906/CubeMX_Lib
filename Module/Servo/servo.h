#ifndef _SERVO_H
#define _SERVO_H

#include "main.h"
#include "DELAY/delay.h"

#define DPOTX9C_CS_PIN GPIO_PIN_10
#define DPOTX9C_INC_PIN GPIO_PIN_11
#define DPOTX9C_UD_PIN GPIO_PIN_12

#define DPOTX9C_CS_GPIO GPIOB
#define DPOTX9C_INC_GPIO GPIOB
#define DPOTX9C_UD_GPIO GPIOB

#define DPOTX9C_CS_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define DPOTX9C_INC_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()
#define DPOTX9C_UD_CLKEN() __HAL_RCC_GPIOB_CLK_ENABLE()

typedef struct _ServoHandle
{
  TIM_HandleTypeDef *htim;
  uint32_t Channel;
  uint16_t delay; // higher is slower
} ServoHandle;

// The PWM configuration should be finished in CubeMX first
// Prescaler should be set first
// 84Mhz -> psc=83 -> 1Mhz
void Servo_Init(ServoHandle *handle, TIM_HandleTypeDef *htim, uint32_t Channel, uint16_t initPos);
void Servo_Start(ServoHandle *handle);
void Servo_Go(ServoHandle *handle, uint16_t pos);
void Servo_SetDelay(ServoHandle *handle, uint16_t delay);

#endif