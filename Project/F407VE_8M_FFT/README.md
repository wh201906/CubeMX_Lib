Continous Conversion Mode and DMA Continuous Requests MUST BE disabled  
DMA should not be in Circular mode  

use __HAL_ADC_GET_FLAG(&hadc1,ADC_FLAG_OVR) to check whether the transfer is completed  
(DMA Interrupt should be disabled to improve performance)