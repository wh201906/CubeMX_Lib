#include "GRIDKEY/gridkey.h"
#include "GRIDKEY/gridkey_gpio.h"

uint8_t isAllKeyReleased=1;

uint8_t GridKey_Scan(uint8_t gridType)
{
    GPIO_PinState gridState[4][4];
    
    GPIO_PinState isKeyDown=
        HAL_GPIO_ReadPin(grp[4],pin[4])||
        HAL_GPIO_ReadPin(grp[5],pin[5])||
        HAL_GPIO_ReadPin(grp[6],pin[6])||
        HAL_GPIO_ReadPin(grp[7],pin[7]);
        
    if(isKeyDown==GPIO_PIN_SET && isAllKeyReleased)
    {
        Delay_ms(20); // prevent jitter
        if(!(isKeyDown==GPIO_PIN_SET && isAllKeyReleased))
            return 0xFFu;
        isAllKeyReleased=0;
        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
                HAL_GPIO_WritePin(grp[j],pin[j],probeState[i][j]);
            for(int j=0;j<4;j++)
                gridState[i][j]=HAL_GPIO_ReadPin(grp[j+4],pin[j+4]);
        }
        for(int i=0;i<4;i++)
            HAL_GPIO_WritePin(grp[i],pin[i],probeState[4][i]);
        for(int i=0;i<16;i++)
        {
            if(*(*gridState+i)==GPIO_PIN_SET)
                return resultMap[gridType][i];
        }
        return 0xFFu;
    }
    else if(isKeyDown==GPIO_PIN_RESET)
    {
        Delay_ms(20); // prevent jitter
        if(!(isKeyDown==GPIO_PIN_RESET))
            return 0xFFu;
        isAllKeyReleased=1;
    }  
    return 0xFFu;
}
