# AD9833 Demo
The SPI2 is configured to Transmit Only Master mode

PC3->MOSI->SDATA
PB13->SCK->SCLK
PB9->NSS->FSYNC

Necessary Parameters:
FirstBit -> MSB
DataSize -> 16 (for further DMA operations)
NSS -> Hardware, Output

Prescaler -> 2(21.0MBit/s)
CPOL -> Low && CPHA -> 1EDGE
or CPOL -> HIGH && CPHA -> 2EDGE