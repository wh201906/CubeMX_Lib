#ifndef _MYFFT_H
#define _MYFFT_H

#include "main.h"
#include "arm_math.h"

// bigger -> more precise -> takes longer time
#define MYFFT_LENGTH 1024

// use hanning window
#define MYFFT_USE_HANNING 1

// init before use
void MyFFT_Init(double sampleRate);

// get raw FFT result and reduce memory cost
// the data[0] ~ data[MYFFT_LENGTH - 1] is a real number array input.
// the data[0] ~ data[MYFFT_LENGTH / 2 - 1] is the FFT result of each freq.
void MyFFT_CalcInPlace(float32_t *data);

// get raw FFT result
// input length: MYFFT_LENGTH
// output length: MYFFT_LENGTH / 2 (without symmetric part)
void MyFFT_Calc(float32_t *input, float32_t *output); 

// get the freq which has the highest amplitude, based on the given sampleRate
double MyFFT_GetPeakFreq(float32_t *data, uint16_t len);

void MyFFT_SetSampleRate(double sampleRate);

// generate a array with (MYFFT_LENGTH) points
// which has (arrayLen) sine waves
void MyFFT_GenerateArray(float32_t *data, double sampleRate, double *freqArray, double *ampArray, uint32_t arrayLen);

#endif

// memory required of CFFT: 3 * FFT_LENGTH * 32bit
/// memory required of RFFT: 2(or 2.5) * FFT_LENGTH * 32bit

// cfft_radix2 and cfft_radix4 are deprecated
// considering that the FFT is often used in processing a real number array,
// the RFFT is a better choice with faster speed and less memory cost.