Tested with armclang V6.15

# float, FPU on, 120M, -O0, DSP source(TIM2_arr=29):

## 4096,with window:
+ init:7.000ms
+ fft:14.66ms
+ transfer:992.0us

## 4096,without window:
+ init:4.780us
+ fft:12.82ms
+ transfer:992.0us

## 1024,with window:
+ init:1.738ms
+ fft:2.940ms
+ transfer:248.4us

## 1024,without window:
+ init:4.420us
+ fft:2.732ms
+ transfer:248.0us

# float, FPU on, 168M, -O0, DSP source(TIM2_arr=104):

## 4096,with window:
+ init:5.000ms
+ fft:10.56ms
+ transfer:709.0us

## 4096,without window:
+ init:4.040us
+ fft:9.220ms
+ transfer:708.0us

## 1024,with window:
+ init:1.244ms
+ fft:2.112ms
+ transfer:177.6us

## 1024,without window:
+ init:3.696us
+ fft:1.960ms
+ transfer:177.6us

# float, FPU on, 120M, -O0, DSP library(TIM2_arr=29):

## 1024,with window:
+ init:1.506ms
+ fft:882.0us
+ transfer:248.4us

## 4096,with window:
+ init:6.030ms
+ fft:4.200ms
+ transfer:992.0us

# float, FPU on, 120M, -O3, DSP source(TIM2_arr=29):

## 1024,with window:
+ init:307.2us
+ fft:867.0us
+ transfer:54.70us

## 4096,with window:
+ init:1.218ms
+ fft:4.060ms
+ transfer:218.4us

# float, FPU on, 120M, -O3, DSP library(TIM2_arr=29):

## 1024,with window:
+ init:308.0us
+ fft:888.0us
+ transfer:54.70us

## 4096,with window:
+ init:1.220ms
+ fft:4.210ms
+ transfer:218.4us