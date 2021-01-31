# CubeMX_Lib
Some snippets for my STM32F4/H7 board  
封装了一些常用代码，仅针对手上的两款主力开发板(F407VE/H750VB)进行编写及测试  
Module文件夹内为可复用的功能模块，Project文件夹内为CubeMX+HAL库编写的项目  
*** 
使用方法：  
1.新建项目文件夹，并从Template文件夹中拷入对应.ioc文件  
2.重命名.ioc文件，打开，配置工程  
3.生成工程后配置编译器版本为6  
4.include目录添加../../../Module  
5.双击Project窗口，配置所需模块  
6.将下载器设为CMSIS-DAP Debugger  
***
工程中提示无法找到HAL库相关函数时检查.ioc文件中软件包的配置情况(选择使用最新版本软件包)  