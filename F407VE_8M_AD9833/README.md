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
  (the hardware NSS will be pull down as soon as the SPE bit is set to 1)

+ Prescaler -> 2(21.0MBit/s)  
+ CPOL -> HIGH && CPHA -> 2EDGE  
(CPOL MUST be set to HIGH, since the FCLK can only be set to LOW when the SCLK is HIGH, in which the transmit hasn't start yet.)  

\* Take a whole day to prove the Hardware NSS doesn't work there. ￣へ￣