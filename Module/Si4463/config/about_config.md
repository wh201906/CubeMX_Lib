Config files Si446x_Config_30M_xxx are copied from manufactor's demo  
The crystal tolerance of my boards is 10ppm  
nIRQ is used as a symbol of RX completed.  

config files 315_OOK_direct_xxx are used for 312.2M remote control  
Use WDS 3.2.11.0 Direct Rx template  
ChangeLog:  
base freq: 315.2M  
crystal tolerance: 10ppm  
Modulation type: OOK  
Data rate: xxx  
Rxdata rate error: 1%~10%  
GPIO0 is the RxClk  
GPIO1 is the RxData  
GPIO2 is the TxData(INPUT)  
GPIO3 is the TxClk  
Switch between Direct Rx and Direct Tx template to apply each other's settings.  