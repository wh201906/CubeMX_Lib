#ifndef _PARAIO_H
#define _PARAIO_H

#include "main.h"

#define PARAIO_DATAIN_GPIO GPIOD
#define PARAIO_DATAIN_CLKEN() __HAL_RCC_GPIOD_CLK_ENABLE()

#endif 