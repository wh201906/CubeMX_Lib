#ifndef _MYGPIO_H_
#define _MYGPIO_H_

// The GPIO_InitTypeDef should be named as "GPIO_InitStruct"
#define MyGPIO_Init(__GPIO__, __PIN__, __LEVEL__)          \
  do                                                       \
  {                                                        \
    HAL_GPIO_WritePin((__GPIO__), (__PIN__), (__LEVEL__)); \
    GPIO_InitStruct.Pin = (__PIN__);                       \
    HAL_GPIO_Init((__GPIO__), &GPIO_InitStruct);           \
  } while (0)

#endif