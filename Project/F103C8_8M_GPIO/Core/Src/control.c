#include "control.h"

// No speed requirement
// Write what I want

// Command format(case insensitive):
// (r/w),(group),(pin),(state)>
// use '>' as the end of a command rather than "\r\n"
// set PA5 HIGH:
// w,a,5,1>
// set PC13 LOW:
// w,c,13,0>
// set PB0~15 HIGH:
// w,b,all,1>
// set all LOW
// w,all,,0>
// read PA7 (the serial will return "1>" or "0>", only two byte)
// r,a,7,>

uint16_t pinList[16] = {
    GPIO_PIN_0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
};

GPIO_TypeDef *grpList[] = {
    GPIOA,
    GPIOB,
    GPIOC,
};

uint32_t unusedPin[][2] = {

    // Key
    {GPIO_KEY, GPIO_PIN_KEY},

    // USART
    {GPIOA, 9},
    {GPIOA, 10},

    // Debug
    {GPIOA, 13},
    {GPIOA, 14},

    // LED
    {GPIO_LED, GPIO_PIN_LED},

    // OSC
    {GPIOC, 14},
    {GPIOC, 15},
};

uint8_t CTRL_IsPinValid(GPIO_TypeDef *grp, uint32_t pin)
{
  uint16_t i;
  // check group
  for (i = 0; i < sizeof(grpList) / sizeof(grpList[0]); i++)
    if (grp == grpList[i])
      break;
  if (i >= sizeof(grpList) / sizeof(grpList[0]))
    return 0;
  // check pin
  if (pin > 0xFF)
    return 0;
  // check unused
  for (i = 0; i < sizeof(unusedPin) / 4 / 2; i++)
  {
    if (grp == (GPIO_TypeDef *)unusedPin[i][0] && pin == unusedPin[i][i])
      return 0;
  }
  return 1;
}

GPIO_TypeDef *CTRL_MapGrp(char *str)
{
  if (strcmp(str, "a") == 0)
    return GPIOA;
  else if (strcmp(str, "b") == 0)
    return GPIOB;
  else if (strcmp(str, "c") == 0)
    return GPIOC;
  else
    return 0;
}

uint8_t CTRL_ReadCmd(char *str)
{
  char mode;
  char group[32];
  char pin[32];
  uint8_t state, pinNum;
  GPIO_TypeDef *G;

  // to lowercase
  for (state = 0; state < strlen(str); state++)
    str[state] = tolower(str[state]);

  // sepereate the command
  sscanf(str, "%c,%s,%s,%u>", &mode, group, pin, &state);

  if (mode == 'w')
  {
    // mapping group
    if (strcmp(group, "all") == 0)
    {
      CTRL_WriteAll(state);
      return 1;
    }
    else if (strcmp(group, "led") == 0)
    {
      CTRL_LED(state);
      return 1;
    }
    else
      G = CTRL_MapGrp(group);

    // pin
    if (strcmp(pin, "all") == 0)
    {
      CTRL_WriteGrp(G, state);
      return 1;
    }
    else
    {
      pinNum = myatoi(pin);
      return CTRL_WritePin(G, pinNum, state);
    }
  }
  else if (mode == 'r')
  {

    if (strcmp(group, "led") == 0)
      return CTRL_SendState(GPIO_LED, GPIO_PIN_LED);
    else
    {
      G = CTRL_MapGrp(group);
      pinNum = myatoi(pin);
      if (!CTRL_IsPinValid(G, pinNum))
        return 0;
      CTRL_SendState(G, pinNum);
    }
  }
  return 0;
}

uint8_t CTRL_SendState(GPIO_TypeDef *grp, uint8_t pin)
{
  return MyUSART1_WriteStr(HAL_GPIO_ReadPin(grp, pinList[pin]) ? "1>" : "0>");
}

uint8_t CTRL_WritePin(GPIO_TypeDef *grp, uint8_t pin, uint8_t state)
{
  if (!CTRL_IsPinValid(grp, pin))
    return 0;
  HAL_GPIO_WritePin(grp, pinList[pin], !!state);
  return 1;
}

void CTRL_WriteGrp(GPIO_TypeDef *grp, uint8_t state)
{
  uint8_t pin;
  for (pin = 0; pin < 16; pin++)
    CTRL_WritePin(grp, pin, state);
}

void CTRL_WriteAll(uint8_t state)
{
  uint8_t i;
  for (i = 0; i < sizeof(grpList) / sizeof(grpList[0]); i++)
    CTRL_WriteGrp(grpList[i], state);
}

void CTRL_LED(uint8_t state)
{
  // SET -> on, RESET -> off
  HAL_GPIO_WritePin(GPIO_LED, GPIO_PIN_LED, state);
}

uint8_t CTRL_KEY(void)
{
  // SET -> pressed, RESET -> released
  // PA0 should be pulled down
  return HAL_GPIO_ReadPin(GPIO_KEY, GPIO_PIN_KEY);
}