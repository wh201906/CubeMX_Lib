# 计时器模块
基于定时器外设计时，F407上精度可达1/84MHz或1/168MHz  
若计时过程中有中断产生，需保证计时器模块的中断具有更高的抢占优先级，否则计时不准  