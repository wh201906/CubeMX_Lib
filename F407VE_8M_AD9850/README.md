# AD9850 Demo

## Pin
+ PB10    RESET
+ PB11    FQUD
+ PB12    WCLK
+ PE7~15  D0~D7

In Serial mode, the data is loaded in D7.
choose serial or parallel mode by MODE_SERIAL in ad9850.c

## About Serial mode
JUST LOOK AT THE DATASHEET CAREFULLY!
D0->1, D1->1, D2->0