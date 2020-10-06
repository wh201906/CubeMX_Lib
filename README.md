Display:I2C总线128x64 OLED屏幕驱动代码，PE4为SCL,PE5为SDA，支持所有可显示ASCII码，支持内容反色/全屏反色，支持亮度调节，支持显示小数及打印字符数
所需Module:OLED, DELAY

GridKey:4*4矩阵键盘代码，可自定义GPIO,改变GPIO顺序,带20ms消抖
所需Module:OLED, DELAY，GRIDKEY

USART:串口读写代码，支持裸byte，字符串，行，自定义结束字符，操作模式类似Arduino
所需Module:OLED, DELAY，GRIDKEY，USART

使用方法：
1.新建项目文件夹，并从Template文件夹中拷入对应.ioc文件
2.重命名.ioc文件，打开，配置工程
3.生成工程后配置编译器版本为6
4.include目录添加../../Module
5.双击Project窗口，配置所需模块
6.将下载器设为CMSIS-DAP Debugger