main.h  
```
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DELAY/delay.h"
#include "UART/myuart.h"
#include "UTIL/util.h"
/* USER CODE END Includes */
```

main.c
```
/* USER CODE BEGIN PV */
MyUARTHandle uart1;
uint8_t uartBuf1[100];
/* USER CODE END PV */
```

```
  /* USER CODE BEGIN 2 */
  Delay_Init(168);
  MyUART_Init(&uart1, USART1, uartBuf1, 100);
```

stm32f4xx_it.c
```
/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */
extern MyUARTHandle uart1;
```

```
  /* USER CODE BEGIN USART1_IRQn 0 */
  MyUART_IRQHandler(&uart1);
```