# Delay
## Pass-through
+ 168M, 4096point, psc=0, arr=83:  
4.100ms
+ 168M, 1024point, psc=0, arr=83:  
1.026ms
+ 168M, 512point, psc=0, arr=83:  
516.0us
+ 168M, 128point, psc=0, arr=83:  
128.8us
+ 168M, 32point, psc=0, arr=83:  
32.8us
+ 168M, 4096point, psc=0, arr=20:  
428.0us (distorting, tested on 5k Sine wave)
+ 168M, 4096point, psc=0, arr=167:  
8.190ms
+ 168M, 512point, psc=0, arr=167:  
1.028ms
+ 168M, 32point, psc=0, arr=167:  
65.80us
## Pass-through, 2Buffer
+ 168M, 1024point, psc=0, arr=83:  
513.0us

# FIR (ARM_MATH_LOOPUNROLL, Coefficients all filled, -O3, CMSIS_DSP_V1.8.0, ArmClang V6.15)

## No DMA, DSP Source
### CopyQ15()+fir_fast_q15()
+ 128point: 112.8us
+ 512point: 448.0us
+ 1024point: 892.0us

### CopyQ15()
+ 128point: 3.108us
+ 512point: 11.82us
+ 1024point: 23.28us

### fir_fast_q15()
+ 128point: 109.8us
+ 512point: 436.0us
+ 1024point: 868.0us

### fir_q15()
+ 128point: 113.6us
+ 512point: 451.0us
+ 1024point: 898.0us

### Q15toF32()+fir_f32()+F32toQ15()
+ 1024point: 2.020ms
+ 512point: 1.010ms
+ 128point: 220.8us

### fir_f32()
+ 1024point: 1.912ms
+ 512point: 956.0us
+ 128point: 206.4us

## No DMA, DSP Library
### CopyQ15()+fir_fast_q15()
+ 128point: 66.5us
+ 512point: 261.6us
+ 1024point: 522.0us

### CopyQ15()
+ 128point: 2.424us
+ 512point: 9.470us
+ 1024point: 18.68us

### fir_fast_q15()
+ 128point: 64.1us
+ 512point: 252.4us
+ 1024point: 504.0us

### Q15toF32()+fir_f32()+F32toQ15()
+ 1024point: 904.0us
+ 512point: 457.0us
+ 128point: 116.0us

### fir_f32()
+ 1024point: 744.0us
+ 512point: 375.0us
+ 128point: 91.2us

## DMA, DSP Library
### CopyQ15()+fir_fast_q15()
+ 512point: 274.4us

### CopyQ15()
+ 512point: 9.360us

### fir_fast_q15()
+ 512point: 253.2us

### fir_q15()
+ 512point: 344.0us