# AD9833 Demo
The SPI2 is configured to Transmit Only Master mode

PC3->MOSI->SDATA
PB13->SCK->SCLK
PB9->NSS->FSYNC

Necessary Parameters:
Prescaler -> 128(328.125KBit/s)
CPOL -> Low
FirstBit -> MSB
DataSize -> 16 (for further DMA operations)
NSS -> Hardware, Output