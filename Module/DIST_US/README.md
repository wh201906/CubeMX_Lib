# 超声波测距模块
编写模块时使用US-015，使用输入捕获来测量Echo型号脉宽，设计测量精度为1us，对应距离误差小于0.5mm。  
考虑到每个状态下Echo电平已知，使用双边缘触发而非上升沿触发后在中断内改为下降沿触发，减少中断内耗时  
设计period为16位以保证最大兼容性，1us测量最大可达约65ms，且可处理定时器溢出情况(UPDATE中断时计数器 += peroid + 1)  

***

1.PA4设置为GPIO输出，下拉，高速，PA5复用为TIM2_CH1,下拉，高速  
2.定时器通道1配置输入捕获，双边缘触发  
3.定时器自动装载值越大越好，预分频系数为(定时器频率 * 10 ^ -6)-1  
4.NVIC中使能相关中断  
5.Dist_US_GetDistI()获取距离放大100倍后的值（整数），Dist_US_GetDistF()获取距离（float小数），函数最快约在1ms左右返回，最慢约在85ms左右返回  
