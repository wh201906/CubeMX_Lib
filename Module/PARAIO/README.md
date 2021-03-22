# 高速并口IO

基于DMA+GPIO完成内存到外部并口设备的IO操作，同时使用PWM产生时钟信号，可用于驱动并口高速ADDA模块  

在STM32F4当中若外设为GPIO则只能使用DMA2,对应定时器只能选用TIM1和TIM8  
HAL库下独立使用DMA时，若单轮DMA传输完成，可能需要HAL_DMA_Init()来重置DMA状态，否则无法开始下一轮传输  
具体寄存器重置情况有待研究  

F407VE目前配置下能达到的最大IO口速度约为21M  
(正弦波最大频率656.225kHz, 对应psc=0, arr=4, pulse=2, 数组长度为32, 656.225 * 32约为21M)  
尝试多种参数组合无法进一步提升频率，猜测可能已经达到DMA/总线速度极限，也可能是单片机与AD/DA的连接较差导致高频信号无法正常通过  

已实现ADC/DAC芯片(受GPIO限制分辨率不能超过16bit):  
+ AD9280 ADC 8bit 32Msps  
+ AD9708 DAC 8bit 125Msps  