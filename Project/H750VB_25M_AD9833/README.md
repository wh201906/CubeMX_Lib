# AD9833 Demo
The SPI2 is configured to Transmit Only Master mode

## Pin
+ PC1->MOSI->SDATA  
+ PB13->SCK->SCLK  
  (Pull Up!!!)  
+ PB14->NSS->FSYNC(Software)  
  or PB12->NSS->FSYNC(Hardware)  

## Necessary Parameters
+ FirstBit -> MSB  
+ DataSize -> 16  
+ NSS -> Software/Hardware  
  (Hardware NSS works there, because the NSS will toggle to high after every transmit)  
+ NSSP Mode -> Enabled
+ Prescaler -> 16(30.0MBit/s)  
+ CPOL -> HIGH && CPHA -> 2EDGE  