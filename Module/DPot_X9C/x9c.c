#include "x9c.h"

#define X9C_CS(__STATE__) HAL_GPIO_WritePin(DPOTX9C_CS_GPIO, DPOTX9C_CS_PIN, (__STATE__))
#define X9C_INC(__STATE__) HAL_GPIO_WritePin(DPOTX9C_INC_GPIO, DPOTX9C_INC_PIN, (__STATE__))
#define X9C_UD(__STATE__) HAL_GPIO_WritePin(DPOTX9C_UD_GPIO, DPOTX9C_UD_PIN, (__STATE__))

void DPotX9C_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  DPOTX9C_CS_CLKEN();
  DPOTX9C_INC_CLKEN();
  DPOTX9C_UD_CLKEN();

  HAL_GPIO_WritePin(DPOTX9C_CS_GPIO, DPOTX9C_CS_PIN, 1);
  GPIO_InitStruct.Pin = DPOTX9C_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(DPOTX9C_CS_GPIO, &GPIO_InitStruct);

  HAL_GPIO_WritePin(DPOTX9C_INC_GPIO, DPOTX9C_INC_PIN, 1);
  GPIO_InitStruct.Pin = DPOTX9C_INC_PIN;
  HAL_GPIO_Init(DPOTX9C_INC_GPIO, &GPIO_InitStruct);

  HAL_GPIO_WritePin(DPOTX9C_UD_GPIO, DPOTX9C_UD_PIN, 1);
  GPIO_InitStruct.Pin = DPOTX9C_UD_PIN;
  HAL_GPIO_Init(DPOTX9C_UD_GPIO, &GPIO_InitStruct);
}

void DPotX9C_Change(int8_t delta) // -100~+100, can be used without Store()
{
  int8_t i;
  X9C_CS(0);
  if (delta < 0)
  {
    X9C_UD(0);
    delta = -delta;
  }
  else
  {
    X9C_UD(1);
  }
  Delay_us(1); // t_CI
  for (i = 0; i < delta; i++)
  {
    X9C_INC(1);
    Delay_us(2); // t_IH
    X9C_INC(0);
    Delay_us(2); // t_IL
  }
}

void DPotX9C_Store(void) // Store current value after a few of Change()
{
  X9C_INC(1);
  Delay_us(2); // t_IC
  X9C_CS(1);
  Delay_ms(20); // t_CPH_STORE
}

void DPotX9C_Return(void) // set CS to HIGH without store, the current value won't change
{
  X9C_INC(0);
  Delay_us(2); // t_IC
  X9C_CS(1);
  Delay_us(1); // t_CPH_NOSTORE
}