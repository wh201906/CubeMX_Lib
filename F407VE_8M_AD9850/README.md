# AD9850 Demo
The SPI2 is configured to Transmit Only Master mode

## Pin
+ PD9   FQUD   (pulldown)  
+ PD10  D0     (pullup)  
+ PD11  D1     (pullup)  
+ PD12  D2     (pulldown)  
+ PD13  RESET  (pulldown)  
(when pullup/pulldown then hold on, remember to change output level)  

The D0~D2 are used to set in Serial mode, the D2 should be low and the D1&D0 should be high  
and the data is loaded in D7

## Necessary Parameters
+ FirstBit -> LSB  
+ DataSize -> 8  
+ NSS -> Software  

