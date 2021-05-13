#include "wavaudio.h"

inline uint8_t isRIFF(char a, char b, char c, char d, uint8_t *data)
{
    // big-endian
    return (data[3] == a && data[2] == b && data[1] == c && data[0] == d);
}

uint8_t Wav_GetWavInfo(WavInfo *info, uint8_t *data)
{
    uint32_t currSize;
    if (!isRIFF('R', 'I', 'F', 'F', data))
        return 0;
    data += 4;
    info->fileLen = *(uint32_t *)data;
    data += 4;
}