# AD9850 Demo
The SPI2 is configured to Transmit Only Master mode

## Pin
+ PE15   FQUD   (pulldown)  
+ PE13   RESET  (pulldown)  
+ PC3    D7  
+ PB13   WCLK  
(when pullup/pulldown then hold on, remember to change output level)  

and the data is loaded in D7

## Necessary Parameters
+ FirstBit -> LSB  
+ DataSize -> 8  
+ NSS -> Software  

## About Serial mode
According to the datasheet, the serial load mode can be enabled in two ways.
+ Set D0(Pin4), D1(Pin3) to EXACTLY the Vdd and set D2(Pin2) to 0 when start up  
  (If the Vdd is 5V, you shouldn't set D0 and D1 to 3.3V otherwise the chip will be in parallel mode.)
+ Make a positive pulse on the FQUD before every load.

Then, according to my test, you just need to make D0~D6 floating, then you can load data in serial mode.  
(And the positive pulse on the FQUD before every load doesn't matter)