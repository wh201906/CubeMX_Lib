#include "key.h"
#include "key_gpio.h"

uint8_t isAllKeyReleased = 1;

void Key_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    #ifdef STM32H750xx
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    #endif

    #ifdef STM32F407xx
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    #endif
}

uint8_t Key_Scan()
{
    int i;
    uint8_t keyDn[KEY_NUM] = {0};
    uint8_t isKeyDown = 0;
    for (i = 0; i < KEY_NUM; i++)
        isKeyDown = isKeyDown || HAL_GPIO_ReadPin(grp[i], pin[i]) == downSt[i];

    if (isKeyDown && isAllKeyReleased)
    {
        Delay_ms(20); // prevent jitter
        for (i = 0; i < KEY_NUM; i++)
            keyDn[i] = HAL_GPIO_ReadPin(grp[i], pin[i]) == downSt[i];
        isKeyDown = 0;
        for (i = 0; i < KEY_NUM; i++)
            isKeyDown = isKeyDown || keyDn[i];
        if (!isKeyDown)
            return 0xFFu;
        isAllKeyReleased = 0;
        for (i = 0; i < KEY_NUM; i++)
            if (keyDn[i])
                return i;
    }
    else if (!isKeyDown)
    {
        if (isKeyDown)
            return 0xFFu;
        isAllKeyReleased = 1;
    }
    return 0xFFu;
}

uint8_t Key_ScanRaw()
{
    int i;
    uint8_t keyDn[KEY_NUM] = {0};
    uint8_t isKeyDown = 0;
    for (i = 0; i < KEY_NUM; i++)
        isKeyDown = isKeyDown || HAL_GPIO_ReadPin(grp[i], pin[i]) == downSt[i];

    if (isKeyDown)
    {
        Delay_ms(20); // prevent jitter
        for (i = 0; i < KEY_NUM; i++)
            keyDn[i] = HAL_GPIO_ReadPin(grp[i], pin[i]) == downSt[i];
        isKeyDown = 0;
        for (i = 0; i < KEY_NUM; i++)
            isKeyDown = isKeyDown || keyDn[i];
        if (!isKeyDown)
            return 0xFFu;
        for (i = 0; i < KEY_NUM; i++)
            if (keyDn[i])
                return i;
    }
    else if (!isKeyDown)
    {
        if (isKeyDown)
            return 0xFFu;
    }
    return 0xFFu;
}