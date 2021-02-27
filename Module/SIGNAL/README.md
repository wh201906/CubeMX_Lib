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


## FFT
针对时域实数列进行计算，比复数FFT节省空间，速度更快  
支持加汉宁窗，从而减轻频谱泄露现象  
加窗与FFT点数需通过宏定义指定，采样率通过初始化参数或者SetSampleRate()确定，可根据频点下标求对应频率  
函数实现尽可能使用CMSIS DSP库以充分利用FPU资源  
（还没做相位计算，因为DSP库内没有arctan()，如果需要可用math.h当中的atan2()计算）  

测试数据：  
带汉宁窗，4096点，F407VE(168MHz)上测试  
初始化：895us  
单次计算：3178us  
不带窗函数，4096点，F407VE(168MHz)上测试  
初始化：2us  
单次计算：2872us  