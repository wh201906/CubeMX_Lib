#ifndef _GRIDKEY_H
#define _GRIDKEY_H
#include "main.h"
#include "DELAY/delay.h"

// return 255 if no key is pressed.

uint8_t GridKey_Scan(uint8_t gridType);
uint8_t GridKey_ScanRaw(uint8_t gridType);

#endif
