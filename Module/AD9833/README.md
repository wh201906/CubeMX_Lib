# AD9833 DDS

使用硬件SPI+软件NSS(FSYNC)方式驱动，基本实现该芯片所有功能驱动  
五种波形：  
+ 正弦波  
+ 三角波  
+ 方波  
+ 二分频方波  

方波为数字输出，Vout取决于供电；正弦波和三角波为DAC输出，Vout为0.6V(峰峰值)  
有两个频率寄存器和相位寄存器供快速切换/复杂调制  
频率精度为25M/2^28，相位精度为2pi/4096
通信速率主要受HAL库单次SPI发送所限制，芯片SPI自身可达40MBit/s的通信速率。  
设计最大频率为12.5M，实测不超过3M最好(10元便宜无复杂滤波模块)  
***  
引脚连接：  
+ MOSI->SDATA  
+ SCK->SCLK  
+ 独立GPIO->FSYNC  

SPI配置：  
+ FirstBit -> MSB  
+ DataSize -> 16  
+ NSS -> Software  
  (注意在头文件中改对应GPIO宏定义)
+ Prescaler -> 2(21.0MBit/s)(对F4, SPI2)  
  (速度不要超过40MBit/s)  
+ CPOL -> HIGH && CPHA -> 2EDGE  
  (AD9833要求从FSYNC下降沿到SCK下降沿的时间窗口，则SCK空闲极性为高时才可满足要求)  

具体可用函数详见头文件