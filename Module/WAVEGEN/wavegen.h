#ifndef _WAVEGEN_H
#define _WAVEGEN_H

#include "main.h"

typedef enum _WaveGen_WaveType
{
    WAVEGEN_WAVETYPE_SQUARE,
    WAVEGEN_WAVETYPE_SINE,
    WAVEGEN_WAVETYPE_RAMP,
    WAVEGEN_WAVETYPE_TRIANGLE,
} WaveGen_WaveType;

void WaveGen_TimerInit(void);
void WaveGen_setPWMState(uint8_t state);


#endif