# 信号发生器模块
通过切换PA5的模式(DAC/PWM)，实现任意波形发生和高频方波输出的功能  
目前含有正弦波/三角波/方波/锯齿波生成功能和PWM输出功能，支持5种波形切换  
缺少频率到定时器参数的转换，缺少自定义波形填充功能，缺少OutputBuffer切换(禁止后高频波形正常但幅值减小(带载能力弱)，使能后高频幅值正常但波形严重失真)  
***  
初始化时配置先只配置TIM2时钟源和DAC_OUT2  
***
一些看上去可用的波形的实测数据：  
H7:Vdd=5V OBuffer:Disable  
Sine(32dot):f=625K Vpp=1.88V  
Sine(32dot):f=288.6K Vpp=2.74V  
Sine(32dot):f=208.3K Vpp=3.04V  
Square(PWM):f=9.231M Vpp=4.88V  
  
F4:Vdd=5V OBuffer:Disable  
Sine(32dot):f=219.0K Vpp=2.94V  
Sine(32dot):f=101.0K Vpp=3.22V  
Square(PWM):f=7.003M Vpp=4.36V  
Square(PWM):f=3.233M Vpp=4.44V  