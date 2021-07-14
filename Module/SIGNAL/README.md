# 信号处理

## Tips:
对于单轮ADC+DMA采集，ADC与DMA应进行如下配置：

ADC:  
ContinuousConvMode = DISABLE;   // 只需要采集一轮数据，且采集需要外部触发条件来控制等间隔  
DMAContinuousRequests = DISABLE;   // F4中对应ADC_CR2_DDS，DMA最后一次传输完成后不再产生新DMA请求  
DMA:  
Init.Mode = DMA_NORMAL; //DMA完成即终止  

此时检测DMA传输是否完成可使用  
__HAL_ADC_GET_FLAG(&hadc1,ADC_FLAG_OVR)  
检测ADC的状态寄存器SR的OVR位，在传输完成后ADC的下一次DMA请求会失效，从而使得OVR位被置1  


## MyFFT
针对时域实数列进行计算，比复数FFT节省空间，速度更快  
支持加汉宁窗(hann)，汉明窗(hamming)，平顶窗(flattop)，三角窗(triang)，布莱克曼窗(blackman)，从而减轻频谱泄露，进行进一步分析  
加窗与FFT点数需通过宏定义指定，采样率通过初始化参数或者SetSampleRate()确定，可根据频点下标求对应频率  
函数实现尽可能使用CMSIS DSP库以充分利用FPU资源  
（还没做相位计算，因为DSP库内没有arctan()，如果需要可用math.h当中的atan2()计算）  

测试数据：(通过反转GPIO口，外部示波器测量脉宽测量)  
带汉宁窗，4096点，F407VE(168MHz)上测试  
初始化：895us  
单次计算：3178us  
不带窗函数，4096点，F407VE(168MHz)上测试  
初始化：2us  
单次计算：2872us  
[更多测试数据](./ffttest.md)

## SigIO
开辟三段内存，轮流完成读ADC，信号处理，写DAC的操作，可用于实时性较强的信号处理过程。利用DMA双缓冲区功能同时处理ADC/DAC数据，在DMA传输完成中断中不断切换下一轮DMA的目标地址，保证信号读入，处理，输出过程不受中断延时影响  

## SigPara
获取信号参数  
已有功能：  
+ 测有效值(基于CMSIS库)  
+ 测低频信号频率(测周法，用定时器输入捕获，可低至0.01Hz，带定时器溢出处理)  
+ 测高频信号频率(测频法，用定时器门控模式，可高达40MHz)  
+ 自动频率测量(结合测周法和测频法，平衡测量用时与最低精度要求)  
+ PWM参数测量(测周法，用定时器输入捕获，测量占空比和频率，不带定时器溢出处理)  
使用时需要在CubeMX当中任意配置至少一个定时器，从而保证生成tim.h