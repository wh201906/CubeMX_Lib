# 异步串口模块
仿Arduino逻辑设计的串口通讯模块，不以CRLF作为唯一结束标识，支持对单字符/定长bytes/string(以0x00结尾)/任意结尾单字符结尾/CRLF结尾的bytes进行收发操作。且收发时保留末尾特征(0x00/任意字符/CRLF)以便于转发操作，可配合[自用串口助手](https://github.com/wh201906/SerialTest)使用  
目前经测试可在F103, L431, F407, H750上使用  

DMA直接使用HAL库即可，此功能模块仅用于多字节传输  
***  
具体初始化方法见头文件


1.在 stm32fxxx_it.c/stm32hxxx_it.c中添加头文件引用  

2.在 stm32fxxx_it.c/stm32hxxx_it.c中串口中断处添加自定义回调函数MyUSARTx_IRQHandler()  

3.CubeMX中记得配置串口的中断(NVIC中配置)  

4.记得调用MyUSARTx_Init()  
***
其实是UART而不是USART，最开始写的时候没注意  
***
务必拉高Rx口以避免干扰(例如复位时Tx端电压浮动)  
已弃用，Module/UART基于LL库实现，使用自定义Handle，单套.c/.h文件即可用于所有USART外设的async模式  