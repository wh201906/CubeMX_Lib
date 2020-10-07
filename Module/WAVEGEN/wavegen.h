#ifndef _WAVEGEN_H
#define _WAVEGEN_H

#include "main.h"
#include "math.h"

#define WAVEGEN_BUFFER_SIZE 4096

typedef enum _WaveGen_WaveType
{
    WAVEGEN_WAVETYPE_SQUARE,
    WAVEGEN_WAVETYPE_SINE,
    WAVEGEN_WAVETYPE_RAMP,
    WAVEGEN_WAVETYPE_TRIANGLE,
} WaveGen_WaveType;

void WaveGen_DACInit(void);
void WaveGen_DMAInit();
void WaveGen_TimerInit(void);
void WaveGen_setPWMState(uint8_t state);
void WaveGen_setDataBuffer(WaveGen_WaveType waveType,uint16_t vpp);


#endif