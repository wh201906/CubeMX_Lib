#ifndef _WAVAUDIO_H
#define _WAVAUDIO_H

#include "main.h"

typedef struct
{
    uint32_t fileLen;
    uint32_t dataLen;
    uint8_t isPCM;
    uint8_t channelNb;
    uint32_t sampleRate;
    uint8_t bitDepth;
    uint8_t *dataAddr = NULL;
} WavInfo;

uint8_t Wav_GetWavInfo(WavInfo *info, uint8_t *data);

#endif