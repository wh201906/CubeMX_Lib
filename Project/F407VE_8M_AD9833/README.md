# AD9833 Demo
The SPI2 is configured to Transmit Only Master mode

## Pin
+ PC3->MOSI->SDATA  
+ PB13->SCK->SCLK  
  (Pull Up!!!)  
+ PB14->NSS->FSYNC(Software)  

## Necessary Parameters
+ FirstBit -> MSB  
+ DataSize -> 16  
  (for further DMA operations)  
  (No way, the hardware NSS is not available)  
+ NSS -> Software  
  (the hardware NSS will be pull down as soon as the SPE bit is set to 1)(on STM32F4)

+ Prescaler -> 2(21.0MBit/s)  
+ CPOL -> HIGH && CPHA -> 1EDGE  
(CPOL MUST be set to HIGH, since the FCLK can only be set to LOW when the SCLK is HIGH, in which the transmit hasn't start yet.)  
(CPHA should be set to 1EDGE(first edge, CPHA=0, sample on negedge), according to the datasheet)  

\* Take a whole day to prove the Hardware NSS doesn't work there(on STM32F4). ￣へ￣  
\* BUT the Hardware NSS works on STM32H7