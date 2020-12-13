#ifndef _DHT11_H
#define _DHT11_H

#include "main.h"
#include "DELAY/delay.h"

// you can read it as fast as you can, but the valid interval is around 1200ms

#define DHT11_GPIO_IN()  {GPIOE->MODER&=~(3<<(2*2));GPIOE->MODER|=0<<(2*2);} //PE2 In
#define DHT11_GPIO_OUT() {GPIOE->MODER&=~(3<<(2*2));GPIOE->MODER|=1<<(2*2);} //PE2 Out
#define DHT11_GPIO_WRITE(n)  (n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_RESET))
#define DHT11_GPIO_READ()    HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2)

void DHT11_Init(void);
uint8_t DHT11_Measure(uint8_t* tempd, uint8_t* tempf, uint8_t* humid, uint8_t* humif); // return whether the result is valid
uint8_t DHT11_Refresh(void); // return whether the values are updated
double DHT11_GetTemp(void); // call Refresh() first;
double DHT11_GetHumid(void); // call Refresh() first;
uint8_t __DHT11_b2c(uint8_t *addr);

#endif
